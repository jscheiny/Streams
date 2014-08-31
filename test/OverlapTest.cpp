#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(OverlapTest, CompileOrder2) {
    using T = NTuple<int, 2>;
    EXPECT_THAT(MakeStream::range(1, 5) | overlap<2>() | to_vector(),
                ElementsAre(T{1,2}, T{2,3}, T{3,4}));
}

TEST(OverlapTest, CompileOrder3) {
    using T = NTuple<int, 3>;
    EXPECT_THAT(MakeStream::closed_range(1, 5) | overlap<3>() | to_vector(),
                ElementsAre(T{1,2,3}, T{2,3,4}, T{3,4,5}));
}

TEST(OverlapTest, CompileOrder4) {
    using T = NTuple<int, 4>;
    EXPECT_THAT(MakeStream::closed_range(1, 6) | overlap<4>() | to_vector(),
                ElementsAre(T{1,2,3,4}, T{2,3,4,5}, T{3,4,5,6}));
}

TEST(OverlapTest, DynamicOrder2) {
    using T = std::deque<int>;
    EXPECT_THAT(MakeStream::range(1, 5) | overlap(2) | to_vector(),
                ElementsAre(T{1,2}, T{2,3}, T{3,4}));
}

TEST(OverlapTest, DynamicOrder3) {
    using T = std::deque<int>;
    EXPECT_THAT(MakeStream::closed_range(1, 5) | overlap(3) | to_vector(),
                ElementsAre(T{1,2,3}, T{2,3,4}, T{3,4,5}));
}

TEST(OverlapTest, DynamicOrder4) {
    using T = std::deque<int>;
    EXPECT_THAT(MakeStream::closed_range(1, 6) | overlap(4) | to_vector(),
                ElementsAre(T{1,2,3,4}, T{2,3,4,5}, T{3,4,5,6}));
}
