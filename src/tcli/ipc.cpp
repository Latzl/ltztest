#include "ipc.hpp"
#include "tcli.hpp"

#include <thread>

#include <signal.h>

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>

namespace tcli {
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

// todo: multiple listener
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

        args_fn_path.clear();
        args_pass2fn.clear();
        bool bArgsIsPath = true;
        for (auto s : *pvec) {
            std::string str(s.c_str());
            if (bArgsIsPath && str == "--") {
                bArgsIsPath = false;
                continue;
            }
            if (bArgsIsPath) {
                args_fn_path.push_back(str);
            } else {
                args_pass2fn.push_back(str);
            }
        }

        reg.run(args_fn_path.begin(), args_fn_path.end(), run_op);
        if (!reg.ok()) {
            list_at(args_fn_path);
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

    std::vector<std::string> args2pass = args_fn_path;
    if (!args_pass2fn.empty()) {
        args2pass.push_back("--");
        args2pass.insert(args2pass.end(), args_pass2fn.begin(), args_pass2fn.end());
    }
    pvec->clear();
    for (auto s : args2pass) {
        string str(s.c_str(), alloc_char_inst);
        pvec->push_back(str);
    }
    cnd->notify_all();

    return 0;
}
}  // namespace ipc
}  // namespace tcli

TCLI_FN_TCLI(server, is_on) {
    try {
        boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_only, tcli::ipc::shm_name.c_str());
        std::cout << "tcli server is on." << std::endl;
    } catch (boost::interprocess::interprocess_exception& ex) {
        std::cout << "tcli server is off." << std::endl;
    }
    return 0;
}