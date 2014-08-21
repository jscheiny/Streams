#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(RepeatTest, Times) {
    EXPECT_THAT(MakeStream::repeat(42, 0) | to_vector(), IsEmpty());
    EXPECT_THAT(MakeStream::repeat(42, 2) | to_vector(), ElementsAre(42, 42));
}

TEST(RepeatTest, InfiniteTimes) {
    EXPECT_THAT(MakeStream::repeat(42) | limit(3) | to_vector(), ElementsAre(42, 42, 42));
}
