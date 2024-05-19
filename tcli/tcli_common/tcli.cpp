#include "tcli.hpp"
#include "tcli_opt.hpp"
#include <signal.h>
#include <iostream>
#include <thread>

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace tcli {

int argc = 0;
char** argv = nullptr;
std::vector<std::string> args;

/* define options of tcli */
TCLI_OPT_F(tcli) {
    boost::program_options::options_description desc("tcli");
    desc.add_options()  //
        ("help,h", boost::program_options::bool_switch(), "Show this message then exit.")  //
        ("list,t", boost::program_options::bool_switch(), "List sub path of current given path then exit.")  //
        ("list-all,T", boost::program_options::bool_switch(), "List all registered function tree then exit. Node with * indicate that a function has registered on this node. Therefore, the path to this node can be the path to excutable function.")  //
        ("fpath,f", boost::program_options::value<std::vector<std::string>>()->default_value(args, "")->multitoken(), "Set function path to execute.")  //
        ("silence,s", boost::program_options::bool_switch(), "Silence mode.")  //
        ("verbose,v", boost::program_options::bool_switch(), "Verbose mode.")  //
        ("listen,l", boost::program_options::bool_switch(), "Listen other tcli process. As tcli server, wait for function path message from client then parse this path and execute.")  //
        ("connect,c", boost::program_options::bool_switch(), "Connect to tcli server. Send function path message to server.")  //
        ;
    auto& opt = tcli::opt::Opt::instance();
    opt.add_description(desc);
    opt.add_pos_description("fpath", -1);
    return 0;
}


namespace ipc {
const std::string shm_name = "ltz_tcli_shm";
const std::string vec_name = "ltz_tcli_vec";
const std::string mtx_name = "ltz_tcli_mtx";
const std::string cnd_name = "ltz_tcli_cnd";
using allocator_char = boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>;
using string = boost::interprocess::basic_string<char, std::char_traits<char>, allocator_char>;
using allocator_vec = boost::interprocess::allocator<string, boost::interprocess::managed_shared_memory::segment_manager>;
using vector = boost::interprocess::vector<string, allocator_vec>;
static bool listening = false;

static void siginit_handler(int signum) {
    std::cout << "SIGINT received, exit." << std::endl;
    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_only, shm_name.c_str());
    boost::interprocess::interprocess_condition* cnd = managed_shm.find<boost::interprocess::interprocess_condition>(cnd_name.c_str()).first;
    listening = false;
    cnd->notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int listen() {
    std::cout << "start listen" << std::endl;
    struct Remover {
        Remover() {
            boost::interprocess::shared_memory_object::remove(shm_name.c_str());
        }
        ~Remover() {
            boost::interprocess::shared_memory_object::remove(shm_name.c_str());
        }
    } remover;
    signal(SIGINT, siginit_handler);

    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create, shm_name.c_str(), 65536);

    const allocator_char alloc_char_inst(managed_shm.get_segment_manager());
    const allocator_vec alloc_vec_inst(managed_shm.get_segment_manager());
    vector* pvec = managed_shm.construct<vector>(vec_name.c_str())(alloc_vec_inst);

    boost::interprocess::interprocess_mutex* mtx = managed_shm.find_or_construct<boost::interprocess::interprocess_mutex>(mtx_name.c_str())();
    boost::interprocess::interprocess_condition* cnd = managed_shm.find_or_construct<boost::interprocess::interprocess_condition>(cnd_name.c_str())();
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*mtx);

    listening = true;
    auto& reg = get_register();
    while (1) {
        cnd->wait(lock);
        if (!listening) {
            break;
        }
        args.clear();
        for (auto s : *pvec) {
            std::string str(s.c_str());
            args.push_back(str);
        }

        reg.run(args.begin(), args.end());
        if (!reg.ok_) {
            std::cout << "possible sub path: " << std::endl;
            list(args);
        }
    }

    return 0;
}

int connect() {
    boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_only, shm_name.c_str());
    boost::interprocess::interprocess_mutex* mtx = managed_shm.find<boost::interprocess::interprocess_mutex>(mtx_name.c_str()).first;
    boost::interprocess::interprocess_condition* cnd = managed_shm.find<boost::interprocess::interprocess_condition>(cnd_name.c_str()).first;
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*mtx);
    const allocator_char alloc_char_inst(managed_shm.get_segment_manager());
    vector* pvec = managed_shm.find<vector>(vec_name.c_str()).first;
    if (!pvec) {
        std::cerr << "can't find vector" << std::endl;
        return -1;
    }

    pvec->clear();
    for (auto s : args) {
        string str(s.c_str(), alloc_char_inst);
        pvec->push_back(str);
    }
    cnd->notify_all();

    return 0;
}
}  // namespace ipc


int main() {
    auto& reg = get_register();
    int r = reg.run(args.begin(), args.end());
    if (!reg.ok_) {
        std::cout << "possible sub path: " << std::endl;
        list(args);
        return -1;
    }
    return r;
}

}  // namespace tcli

/* define function below */
#define TCLI_F_TCLI(...) TCLI_F(tcli, __VA_ARGS__)
TCLI_F_TCLI(toStr_registered_debug) {
    std::cout << tcli::get_register().toStr_registered(1, "root") << std::endl;
    return 0;
}

TCLI_F_TCLI(echo_args) {
    for (const auto& s : lpiArgs) {
        std::cout << s << std::endl;
    }
    return 0;
}

TCLI_F_TCLI(server, is_on) {
    try {
        boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_only, tcli::ipc::shm_name.c_str());
        std::cout << "tcli server is on." << std::endl;
    } catch (boost::interprocess::interprocess_exception& ex) {
        std::cout << "tcli server is off." << std::endl;
    }
    return 0;
}