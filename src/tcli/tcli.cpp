#include "tcli.hpp"
#include "opt.hpp"
#include "gtest.hpp"
#include "ipc.hpp"
#include "log.hpp"
#include "utility.hpp"

#include <iostream>

#include <ltz/proc_init/register.hpp>
#include <boost/program_options.hpp>

namespace tcli {

int argc_raw = 0;
char** argv_raw = nullptr;
std::vector<std::string> args_pass2fn, args_fn_path;

std::string node::get_info() {
    std::string s, sInfo;
    s += "t{";

    auto append = [&sInfo](const std::string& s) {
        if (!sInfo.empty()) {
            sInfo += ", ";
        }
        sInfo += s;
    };

    sInfo += "f";
    if (!tcli_desc.empty()) {
        append("d");
    }

    s += sInfo;
    s += "}";

    return s;
}

ltz::proc_init::fn_reg& get_register() {
    auto& reg = LTZ_PI_FN_GET_REG(tcli);
    return reg;
}

const std::string LIST_HEADER = "Candidate nodes:";

std::vector<std::string> get_registered_nodes_at(const std::vector<std::string>& vArgsAsFnPath) {
    namespace lpi = ltz::proc_init;
    using lpir_reg = lpi::fn_reg::reg_p;
    std::vector<std::string> vNodes;

    auto fn = [&vNodes](lpir_reg::reg_tree& tree, const lpir_reg::for_each_ctx& ctx) { vNodes.push_back(ctx.node_name); };

    tcli::get_register().for_each_at(vArgsAsFnPath, fn, 1);
    if (!tcli::get_register().ok()) {
        std::cerr << "something wrong?" << std::endl;
    }

    return vNodes;
}

std::string toStr_candidate_nodes(const std::vector<std::string>& vCandidate) {
    std::string sOutput;
    for (auto& s : vCandidate) {
        sOutput += s + "\t";
    }
    if (!sOutput.empty()) {
        sOutput.pop_back();
    }
    if (!args_pass2fn.empty()) {
        std::cout << "With path: ";
        std::copy(args_fn_path.begin(), args_fn_path.end(), std::ostream_iterator<std::string>(std::cout, "/"));
        std::cout << ", " << LIST_HEADER << "\n";
    } else {
        std::cout << LIST_HEADER << "\n";
    }
    return sOutput;
}

void list_at(const std::vector<std::string>& vArgsAsFnPath) {
    std::vector<std::string> v = get_registered_nodes_at(vArgsAsFnPath);
    std::string sOutput = toStr_candidate_nodes(v);
    std::cout << sOutput << std::endl;
}

using list_flag_t = uint32_t;
struct list_flag {
    static const list_flag_t func = (1 << 0);
    static const list_flag_t desc = (1 << 1);
    static const list_flag_t addr = (1 << 2);

    static const list_flag_t dflt = (func | desc);
};
std::string get_registered_node_all(list_flag_t flag) {
    namespace lpi = ltz::proc_init;
    using lpir_reg = lpi::fn_reg::reg_p;
    std::stringstream ss;
    ss << "tcli\n";
    auto fn = [&ss, flag](lpir_reg::reg_tree& tree, const lpir_reg::for_each_ctx& ctx) {
        lpi::fn::node* lpif_node = tree.get_value<lpi::fn::node*>();
        auto pNode = dynamic_cast<basic_node*>(lpif_node);

        ss << std::string((ctx.depth + 1) * 2, ' ') << ctx.node_name;

        if (pNode) {
            ss << " " << pNode->get_info();
        }
        ss << "\n";
    };

    tcli::get_register().for_each(fn);

    std::string s = std::move(ss.str());
    if (s.back() == '\n') {
        s.pop_back();
    }

    return s;
}

void list_all() {
    std::string s = get_registered_node_all(list_flag::dflt);
    if (!s.empty()) {
        std::cout << s << std::endl;
    }
}


void print_desc(const std::vector<std::string>& vArgsAsFnPath) {
    auto pr = get_register().get(vArgsAsFnPath.begin(), vArgsAsFnPath.end());
    auto lpif_node = pr.first;
    auto pNode = dynamic_cast<tcli::node*>(lpif_node);
    if (!pNode || pNode->tcli_desc.empty()) {
        return;
    }
    std::cout << pNode->tcli_desc << std::endl;
}


template <typename InputIt, typename std::enable_if<std::is_same<typename std::iterator_traits<InputIt>::value_type, std::string>::value>::type* = nullptr>
std::string path2str(InputIt first, InputIt last) {
    std::stringstream ss;
    while (first != last) {
        ss << *first << "/";
        first = std::next(first);
    }
    std::string s = ss.str();
    if (s.size()) {
        s.pop_back();
    }
    return s;
}

int run_op(ltz::proc_init::fn::node& lpif_node) {
    auto& nd = dynamic_cast<basic_node&>(lpif_node);
    int nRet = 0;
    std::stringstream ss;

    // todo path2str belong to reg
    ss << "======== " << path2str(args_fn_path.begin(), args_fn_path.end());
    if (!args_pass2fn.empty()) {
        ss << " -- " << ltz::str::join(args_pass2fn.begin(), args_pass2fn.end(), " ");
    }
    ss << " ========";
    std::cout << ss.str() << std::endl;
    ss.str("");


    Timer timer;
    nRet = nd.lpif_main(args_pass2fn);
    ss << timer.end().toStr_pass_time();

    std::cout << "======== " << "return value: " << nRet << ", time cost: " << ss.str()  //
              << " ========" << std::endl;
    return nRet;
}

/*
    @brief Divide argv into two part at first '--'
    @return Pair of vectors, first for args before '--', as seconde after. Note pair.first doesn't include argv[0]
 */
static std::pair<std::vector<std::string>, std::vector<std::string>> divide_argv(int argc, char* argv[]) {
    std::vector<std::string> v_raw(argv + 1, argv + argc), v_front, v_behind;
    auto it = std::find(v_raw.begin(), v_raw.end(), "--");
    v_front.assign(v_raw.begin(), it);
    if (it != v_raw.end()) {
        v_behind.assign(std::next(it), v_raw.end());
    }
    return {v_front, v_behind};
}

static std::pair<std::vector<std::string>, std::vector<std::string>> parse2args(const std::vector<std::string>& vArgsAsFnPath, const std::vector<std::string>& vArgsPass2Fn) {
    std::vector<std::string> vArgsAsFnPath_out, vArgsPass2Fn_out;
    auto& reg = get_register();
    auto pr = reg.get(vArgsAsFnPath.begin(), vArgsAsFnPath.end());
    auto it = pr.second;
    if (it == vArgsAsFnPath.end()) {
        return {vArgsAsFnPath, vArgsPass2Fn};
    }
    vArgsAsFnPath_out.assign(vArgsAsFnPath.begin(), it);
    vArgsPass2Fn_out.assign(it, vArgsAsFnPath.end());
    vArgsPass2Fn_out.insert(vArgsPass2Fn_out.end(), vArgsPass2Fn.begin(), vArgsPass2Fn.end());
    return {vArgsAsFnPath_out, vArgsPass2Fn_out};
}

int main(int argc, char* argv[]) {
    argc_raw = argc;
    argv_raw = (char**)argv;

    auto& opt = tcli::opt::opt;

    std::vector<std::string> vArgsAsFnPath, vArgsPass2Fn;
    auto pr_divided = divide_argv(argc, argv);
    vArgsAsFnPath = pr_divided.first;
    vArgsPass2Fn = pr_divided.second;

    opt.init(vArgsAsFnPath);
    opt.parse();

    auto& vm = opt.vm_;

    /* tcli */
    vArgsAsFnPath = vm["fpath"].as<std::vector<std::string>>();
    auto pr_parsed = parse2args(vArgsAsFnPath, vArgsPass2Fn);
    args_fn_path = pr_parsed.first;
    args_pass2fn = pr_parsed.second;

    if (vm["list"].as<bool>()) {
        tcli::list_at(args_fn_path);
        return 0;
    }
    if (vm["list-all"].as<bool>()) {
        tcli::list_all();
        return 0;
    }
    if (vm["print-desc"].as<bool>()) {
        tcli::print_desc(args_fn_path);
        return 0;
    }

    if (vm["listen"].as<bool>()) {
        return tcli::ipc::listen();
    }
    if (vm["connect"].as<bool>()) {
        return tcli::ipc::connect();
    }

    if (vm["help"].as<bool>() || args_fn_path.empty()) {
        std::cout << opt.help() << std::endl;
        std::cout << "registered fuction tree: " << std::endl;
        tcli::list_all();
        return 0;
    }

    // todo abstract as plugin
    // todo tcli server won't took over by gtest, fix this in future
    if (gtest::is_need_take_over()) {
        return gtest::main();
    }

    auto& reg = get_register();
    int r = reg.run(args_fn_path.begin(), args_fn_path.end(), run_op);
    if (!reg.ok()) {
        list_at(args_fn_path);
        return -1;
    }
    return r;
}


}  // namespace tcli

TCLI_FN_TCLI(list_all_nodes) {
    tcli::list_all();
    return 0;
}

TCLI_FN_TCLI(echo_args) {
    for (const auto& s : lpif_args) {
        std::cout << s << std::endl;
    }
    return 0;
}
