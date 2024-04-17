/*
    @brief tc: t(est) c(li)
 */
#ifndef _tcbase_hpp_
#define _tcbase_hpp_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include <boost/preprocessor.hpp>

namespace tc {

using func = void (*)(std::vector<std::string> tcArgs);

struct Node;
using SpNode = std::shared_ptr<Node>;
struct Node {
    Node(std::string nm) : name_(nm) {}
    std::string name_;
    func func_{nullptr};
    std::map<std::string, SpNode> children_;
    /* err */
    bool ok_{true};
    std::string errMsg_;
    inline void setOk() {
        ok_ = true;
        errMsg_ = "";
    }
    inline void setErr(const std::string& msg) {
        ok_ = false;
        errMsg_ = msg;
    }
    /*
        @brief Insert a node, the variadics describe the path where node should be insert to. If the node on the path yet exist, create it.
        @note If node of same path is exist, node won't be insert.
     */
    template <typename Arg, typename... Args>
    inline void insert(SpNode spTcNode, Arg argLeft, Args... args) {
        auto it = children_.find(argLeft);
        if (it == children_.end()) {
            it = children_.emplace(argLeft, SpNode(new Node(argLeft))).first;
        }
        it->second->insert(spTcNode, args...);
    }
    template <typename... Args>
    inline void insert(SpNode spTcNode) {
        auto pr = children_.emplace(spTcNode->name_, spTcNode);
        setOk();
        if (!pr.second) {
            setErr("duplicate node name: " + spTcNode->name_);
        }
    }
    /*
        @brief Get the func describe by the path.
        @brief args will be changed
     */
    inline func getFunc(std::vector<std::string>& args) {
        setOk();
        if (args.empty()) {
            return func_;
        }
        auto it = children_.find(args[0]);
        if (it == children_.end()) {
            return func_;
        }
        args.erase(args.begin());
        return it->second->getFunc(args);
    }
    /*
        @brief Get children name under current node
     */
    inline std::string getChildrenName() {
        setOk();
        std::string s;
        for (auto& it : children_) {
            s += it.first + "\n";
        }
        if (s.size() > 0) {
            s.pop_back();
        }
        return s;
    }
    /*
        @brief Print optional command depending on path
        @note Will change args
     */
    inline void listLastCmdOption(std::vector<std::string>& args) {
        setOk();
        if (args.empty()) {
            std::string s = getChildrenName();
            if (!s.empty()) {
                std::cout << s << std::endl;
            }
            return;
        }
        auto it = children_.find(args[0]);
        if (it == children_.end()) {
            setErr("unknown command: " + args[0]);
            return;
        }
        args.erase(args.begin());
        it->second->listLastCmdOption(args);
    }
};

inline Node& getRoot() {
    static Node root("root");
    return root;
}
}  // namespace tc

#define TC_SEQ_LAST_ELEM(seq) BOOST_PP_SEQ_ELEM(BOOST_PP_SUB(BOOST_PP_SEQ_SIZE(seq), 1), seq)
#define TC_VARIDADIC_LAST(...) TC_SEQ_LAST_ELEM(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
#define TC_OPR1(r, data, elem) , BOOST_PP_STRINGIZE(elem)
#define TC_SEQ_TO_STRS(seq) BOOST_PP_SEQ_FOR_EACH(TC_OPR1, _, seq)

#define TC_OPR(s, state, x) BOOST_PP_CAT(BOOST_PP_CAT(state, x), _)
#define TC_GEN_NAME(prefix, ...) BOOST_PP_CAT(prefix##_, BOOST_PP_SEQ_FOLD_LEFT(TC_OPR, BOOST_PP_CAT(BOOST_PP_SEQ_HEAD(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)), _), BOOST_PP_SEQ_TAIL(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))))
#define TC_GEN_NAME_FUNC(...) TC_GEN_NAME(_tcf, __VA_ARGS__)
#define TC_GEN_NAME_REGTAG(...) TC_GEN_NAME(_tcr, __VA_ARGS__)
#define TC_GEN_NAME_REG(...) TC_GEN_NAME(tcr, __VA_ARGS__)

#define TCFUNC(...)                                                                                                \
    namespace tc {                                                                                                 \
    void TC_GEN_NAME_FUNC(__VA_ARGS__)(std::vector<std::string> tcArgs);                                           \
    struct TC_GEN_NAME_REGTAG(__VA_ARGS__) {                                                                       \
        TC_GEN_NAME_REGTAG(__VA_ARGS__)() {                                                                        \
            SpNode spNode = std::make_shared<Node>(BOOST_PP_STRINGIZE(TC_VARIDADIC_LAST(__VA_ARGS__)));            \
            spNode->func_ = TC_GEN_NAME_FUNC(__VA_ARGS__);                                                         \
            getRoot().insert(spNode TC_SEQ_TO_STRS(BOOST_PP_SEQ_POP_BACK(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))); \
        }                                                                                                          \
    } TC_GEN_NAME_REG(__VA_ARGS__);                                                                                \
    }                                                                                                              \
    void tc::TC_GEN_NAME_FUNC(__VA_ARGS__)(std::vector<std::string> tcArgs)
#endif
