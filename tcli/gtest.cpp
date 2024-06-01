#include <tcli/gtest.hpp>

namespace tcli {
namespace gtest {
int test_with_filter(const std::string& suit, const std::string& test) {
    ::testing::InitGoogleTest(&argc, argv);
    std::string filter, suit_ = suit, test_ = test;
    if (suit.empty()) {
        suit_ = "*";
    }
    if (test.empty()) {
        test_ = "*";
    }
    filter = suit_ + "." + test_;
    ::testing::GTEST_FLAG(filter) = filter.c_str();
    return RUN_ALL_TESTS();
}
}  // namespace gtest
}  // namespace tcli