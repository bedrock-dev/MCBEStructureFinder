
#include <cassert>

#include <vector>
#include <functional>

#define TEST namespace {std::vector<std::function<void()>> tests;}

#define  ADD_TEST(F_CALL) tests.emplace_back([](){F_CALL;})

#define RUN_ALL_TEST for(auto &t:tests)t();



#define EXPECT_TRUE(val) assert(val)
#define EXPECT_FALSE(val) assert(!(val))

#define EXPECT_EQ(x, y) assert((x) == (y))




