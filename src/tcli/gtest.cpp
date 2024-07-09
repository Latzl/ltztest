#include <tcli/gtest.hpp>
#include "gtest.hpp"
#include "tcli.hpp"

namespace tcli {
namespace gtest {

std::string node::toStr_char(node::type_t t) {
    switch (t) {
        case node::test_type::case_:
            return "c";
        default:
            throw std::domain_error("unknown type: " + std::to_string(t));
    }
}
std::string node::get_info() {
    std::string s, sInfo;
    s += "g{";

    auto append = [&sInfo](const std::string& s) {
        if (!sInfo.empty()) {
            sInfo += ", ";
        }
        sInfo += s;
    };

    append(toStr_char(type));

    s += sInfo;
    s += "}";

    return s;
}

bool is_need_take_over() {
    bool bNeed = !args_fn_path.empty() && args_fn_path[0] == "gtest";
    return bNeed;
}

static int test_all() {
    ::testing::InitGoogleTest(&argc_raw, argv_raw);
    return RUN_ALL_TESTS();
}

std::string toStr(test_type type) {
    switch (type) {
        case test_type::all:
            return "all";
        case test_type::suit:
            return "suit";
        case test_type::case_:
            return "case";
        case test_type::subs:
            return "subs";
        case test_type::mix:
            return "mix";
        case test_type::unknown:
        default:
            return "unknown";
    }
}


Filter::Filter(const std::vector<std::string>& path, const std::vector<std::string>& args) : nodePath(path) {
    // gtest [...] <suit> <case> [*|**]
    if (nodePath.empty() || nodePath.front() != "gtest") {
        return;
    }

    itRoot_ = nodePath.begin();
    itSuit_ = nodePath.end();
    itCase_ = nodePath.end();

    if (trans2all()) {
        bReady_ = true;
        return;
    }

    auto pr = get_register().get_tree_node(nodePath.begin(), nodePath.end());
    auto itr = pr.second;
    auto& node = pr.first;
    pRegTree_ = &node;

    // input path not match registered node path
    if (itr != nodePath.end()) {
        return;
    }


    /* if last node of path is:
            1. a normal name, if
                1.1 last named node has exist node value, to case
                1.2 last named node has direct test case, to suit
                1.3 last named node has no direct test case, to subs
                or
            2. '*', to subs, or
            3. '**', to suit (if exist) and subs and every subs that path match the suit prefix
        In 2 and 3 is for last named node is on path to subs but match suit name
     */
    if (!args.empty()) {
        if (args.front() == ".") {
            if (trans2subs()) {
                bReady_ = true;
                return;
            }
        } else if (args.back() == "..") {
            if (trans2mix()) {
                bReady_ = true;
                return;
            }
        }
    } else {
        // case
        if (trans2case()) {
            bReady_ = true;
            return;
        }

        // suit
        if (trans2suit()) {
            bReady_ = true;
            return;
        }

        // subs
        if (trans2subs()) {
            bReady_ = true;
            return;
        }
    }
}

bool Filter::ready() const {
    return bReady_;
}

std::string Filter::get_filter() const {
    return filter_;
}

test_type Filter::get_type() const {
    return type_;
}

bool Filter::trans2all() {
    if (nodePath.size() != 1) {
        return false;
    }

    type_ = test_type::all;

    filter_ = "*.*";

    return true;
}
bool Filter::trans2case() {
    auto plpif_node = pRegTree_->get_value<lpif::node*>(lpif_reg::reg_p::to_entry_translator());
    // exist node indicate test case defined at this path
    if (!plpif_node) {
        return false;
    }

    type_ = test_type::case_;

    itSuit_ = nodePath.end() - 2;
    itCase_ = nodePath.end() - 1;
    std::string suit_name = ltz::str::join(itRoot_, itSuit_, "_") + "_" + *(itSuit_);
    std::string case_name = *itCase_;
    filter_ = suit_name + "." + case_name;

    return true;
}
bool Filter::trans2suit() {
    bool bChildrenHasCase = false;
    for (auto pr : *pRegTree_) {
        if (pr.second.get_value<lpif::node*>(lpif_reg::reg_p::to_entry_translator())) {
            bChildrenHasCase = true;
            break;
        }
    }
    if (!bChildrenHasCase) {
        return false;
    }

    type_ = test_type::suit;

    itSuit_ = nodePath.end() - 1;
    std::string suit_name = ltz::str::join(itRoot_, itSuit_, "_") + "_" + *itSuit_;
    std::string case_name = "*";
    filter_ = suit_name + "." + case_name;

    return true;
}
bool Filter::trans2subs() {
    if (pRegTree_->empty()) {  // no children
        return false;
    }

    type_ = test_type::subs;

    std::string suit_name = ltz::str::join(itRoot_, nodePath.end(), "_") + "_*";
    std::string case_name = "*";
    filter_ = suit_name + "." + case_name;

    return true;
}
bool Filter::trans2mix() {
    type_ = test_type::mix;

    std::string suit_name = ltz::str::join(itRoot_, itSuit_, "_") + "*";
    std::string case_name = "*";
    filter_ = suit_name + "." + case_name;

    return true;
}

int main() {
    Filter filter(args_fn_path, args_pass2fn);

    if (!filter.ready()) {
        list_at(args_fn_path);
        return -1;
    }

    std::cout << "type: " << toStr(filter.get_type()) << "\n"
              << "filter: " << filter.get_filter() << std::endl;

    switch (filter.get_type()) {
        case test_type::all:
            return test_all();
        case test_type::suit:
        case test_type::case_:
        case test_type::subs:
        case test_type::mix:
            return test_with_filter(filter.get_filter());
        case test_type::unknown:
        default:
            break;
    }

    list_at(args_fn_path);

    return -1;
}

int test_with_filter(const std::string& filter) {
    ::testing::InitGoogleTest(&argc_raw, argv_raw);
    ::testing::GTEST_FLAG(filter) = filter.c_str();
    return RUN_ALL_TESTS();
}

int test_with_filter(const std::string& suit_name, const std::string& case_name) {
    std::string filter, suit_name_ = suit_name, case_name_ = case_name;
    if (suit_name.empty()) {
        suit_name_ = "*";
    }
    if (case_name.empty()) {
        case_name_ = "*";
    }
    filter = suit_name_ + "." + case_name_;
    return test_with_filter(filter);
}

}  // namespace gtest
}  // namespace tcli