#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(SliceTest, Slice) {
    EXPECT_THAT(MakeStream::range(0, 10) | slice(0, 5) | to_vector(),
                ElementsAre(0, 1, 2, 3, 4));
    EXPECT_THAT(MakeStream::range(0, 10) | slice(2, 8, 2) | to_vector(),
                ElementsAre(2, 4, 6));
    EXPECT_THAT(MakeStream::range(0, 10) | slice(1, 8, 2) | to_vector(),
                ElementsAre(1, 3, 5, 7));
}

TEST(SliceTest, SliceToEnd) {
    EXPECT_THAT(MakeStream::range(1, 11) | slice_to_end(5, 2) | to_vector(),
                ElementsAre(6, 8, 10));
    EXPECT_THAT(MakeStream::range(0, 5) | slice_to_end(7, 10) | to_vector(),
                IsEmpty());
}

TEST(SliceTest, Skip) {
    EXPECT_THAT(MakeStream::range(1, 8) | skip(3) | to_vector(),
                ElementsAre(4, 5, 6, 7));
    EXPECT_THAT(MakeStream::range(1, 5) | skip(10) | to_vector(),
                IsEmpty());
    EXPECT_THAT(MakeStream::empty<int>() | skip(5) | to_vector(),
                IsEmpty());
}

TEST(SliceTest, Limit) {
    EXPECT_THAT(MakeStream::counter(0) | limit(5) | to_vector(),
                ElementsAre(0, 1, 2, 3, 4));
    EXPECT_THAT(MakeStream::range(0, 3) | limit(5) | to_vector(),
                ElementsAre(0, 1, 2));
    EXPECT_THAT(MakeStream::empty<int>() | limit(20) | to_vector(),
                IsEmpty());
}
