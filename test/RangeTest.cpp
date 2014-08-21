#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(RangeTest, Default) {
    EXPECT_THAT(MakeStream::range(0, 5) | to_vector(),
                ElementsAre(0, 1, 2, 3, 4));
}

TEST(RangeTest, Increment) {
    EXPECT_THAT(MakeStream::range(0, 6, 3) | to_vector(),
                ElementsAre(0, 3));

    EXPECT_THAT(MakeStream::range(0, 7, 3) | to_vector(),
                ElementsAre(0, 3, 6));

    int increment = 5;
    const int& inc_ref = increment;
    EXPECT_THAT(MakeStream::range(0, 11, inc_ref) | to_vector(),
                ElementsAre(0, 5, 10));
}

TEST(ClosedRangeTest, Default) {
    EXPECT_THAT(MakeStream::closed_range(0, 5) | to_vector(),
                ElementsAre(0, 1, 2, 3, 4, 5));
}

TEST(ClosedRangeTest, Increment) {
    EXPECT_THAT(MakeStream::closed_range(0, 6, 3) | to_vector(),
                ElementsAre(0, 3, 6));

    EXPECT_THAT(MakeStream::closed_range(0, 7, 3) | to_vector(),
                ElementsAre(0, 3, 6));

    int increment = 5;
    const int& inc_ref = increment;
    EXPECT_THAT(MakeStream::closed_range(0, 11, inc_ref) | to_vector(),
                ElementsAre(0, 5, 10));
}
