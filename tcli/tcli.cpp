#include <tcli/tcli.hpp>
#include <tcli/opt.hpp>
#include <signal.h>
#include <iostream>
#include <thread>
#include <utility>

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


// ltz::proc_init::Register& get_register() {
//     return ltz::proc_init::get_register("tcli");
// }

void list(const std::vector<std::string>& vPath) {
    // std::string s = get_register().list_children(v_path.begin(), v_path.end());
    std::string s = TCLI_GET_REG().list_children(vPath.begin(), vPath.end());
    if (s.size()) {
        std::cout << s << std::endl;
    }
}

void list_all() {
    // std::string s = get_register().toStr_registered(0, "tcli");
    std::string s = TCLI_GET_REG().toStr_registered();
    if (s.size()) {
        std::cout << s << std::endl;
    }
}

void prompt(const std::vector<std::string>& vPath) {
    // auto pr = get_register().get(vPath.begin(), vPath.end());
    auto pr = TCLI_GET_REG().get(vPath.begin(), vPath.end());
    auto pNode = pr.first;
    if (!pNode) {
        return;
    }
    auto& node = *dynamic_cast<tcli::node*>(pNode);
    if (node.desc.empty()) {
        return;
    }
    std::cout << node.desc << std::endl;
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
    auto& reg = TCLI_GET_REG();
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
        if (!reg.ok()) {
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

template <typename It, typename std::enable_if<std::is_same<typename std::iterator_traits<It>::value_type, std::string>::value>::type* = nullptr>
std::string path2str(It itl, It itr) {
    std::stringstream ss;
    while (itl != itr) {
        ss << *itl << "/";
        itl = std::next(itl);
    }
    std::string s = ss.str();
    if (s.size()) {
        s.pop_back();
    }
    return s;
}

int run_op(ltz::proc_init::fn::node& lpif_node, std::vector<std::string>::iterator itl, std::vector<std::string>::iterator itr, std::vector<std::string>::iterator itm) {
    auto& nd = dynamic_cast<node&>(lpif_node);
    int nRet = 0;
    std::stringstream ss;

    ss << "======== " << path2str(itl, itm);
    if (itm != itr) {
        std::vector<std::string> v(itm, itr);
        ss << " " << ltz::str::join(v.begin(), v.end(), " ");
    }
    ss << " ========";
    std::cout << ss.str() << std::endl;
    ss.str("");

    Timer timer{};
    nRet = nd.lpif_main(std::vector<std::string>{itm, itr});
    ss << timer.end().report();

    std::cout << "======== "
              << "return value: " << nRet << ", time cost: " << ss.str()  //
              << " ========" << std::endl;
    return nRet;
}

int main(int argc, char* argv[]) {
    tcli::argc = argc;
    tcli::argv = (char**)argv;

    auto& opt = tcli::opt::opt;
    opt.init(argc, argv);
    opt.parse();

    auto& vm = opt.vm_;

    /* tcli */
    tcli::args = vm["fpath"].as<std::vector<std::string>>();
    if (vm["list"].as<bool>()) {
        tcli::list(tcli::args);
        return 0;
    }
    if (vm["list-all"].as<bool>()) {
        tcli::list_all();
        return 0;
    }
    if (vm["prompt"].as<bool>()) {
        tcli::prompt(tcli::args);
        return 0;
    }

    if (vm["listen"].as<bool>()) {
        return tcli::ipc::listen();
    }
    if (vm["connect"].as<bool>()) {
        return tcli::ipc::connect();
    }

    if (vm["help"].as<bool>() || tcli::args.empty()) {
        std::cout << opt.get_help() << std::endl;
        std::cout << "registered fuction tree: " << std::endl;
        tcli::list_all();
        return 0;
    }

    auto& reg = TCLI_GET_REG();
    int r = reg.run(args.begin(), args.end(), run_op);
    if (!reg.ok()) {
        std::cout << "possible sub path: " << std::endl;
        list(args);
        return -1;
    }
    return r;
}

/* define options of tcli */
TCLI_OPT_FN(tcli_opt) {
    boost::program_options::options_description desc{"tcli_opt"};
    desc.add_options()  //
        ("help,h", boost::program_options::bool_switch(), "Show this message then exit.")  //
        ("list,t", boost::program_options::bool_switch(), "List sub path of current given path then exit.")  //
        ("list-all,T", boost::program_options::bool_switch(), "List all registered function tree then exit. Node with * indicate that a function has registered on this node. Therefore, the path to this node can be the path to excutable function.")  //
        // todo: deal with problem that pass args need double '--'
        ("fpath,f", boost::program_options::value<std::vector<std::string>>()->default_value(args, "")->multitoken(), "Set function path to execute.")  //
        ("prompt,p", boost::program_options::bool_switch(), "Print the corresponding prompt description for the function path. Only TCLI_SET_PROMPT() specified by function path was used will take effect.")("silence,s", boost::program_options::bool_switch(), "Silence mode.")  //
        ("verbose,v", boost::program_options::bool_switch(), "Verbose mode.")  //
        ("listen,l", boost::program_options::bool_switch(), "Listen other tcli process. As tcli server, wait for function path message from client then parse this path and execute.")  //
        ("connect,c", boost::program_options::bool_switch(), "Connect to tcli server. Send function path message to server.")  //
        ;
    auto& opt = tcli::opt::opt;
    opt.add_description(desc);
    opt.add_pos_description("fpath", -1);
    return 0;
}

}  // namespace tcli

/* define test command below */
#define TCLI_FN_TCLI(...) TCLI_FN(tcli, __VA_ARGS__)

TCLI_FN_TCLI(toStr_registered_debug) {
    std::cout << TCLI_GET_REG().toStr_registered("root") << std::endl;
    return 0;
}

TCLI_FN_TCLI(echo_args) {
    for (const auto& s : lpif_args) {
        std::cout << s << std::endl;
    }
    return 0;
}

TCLI_FN_TCLI(server, is_on) {
    try {
        boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_only, tcli::ipc::shm_name.c_str());
        std::cout << "tcli server is on." << std::endl;
    } catch (boost::interprocess::interprocess_exception& ex) {
        std::cout << "tcli server is off." << std::endl;
    }
    return 0;
}