#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(EmptyTest, MakeStream) {
    EXPECT_THAT(MakeStream::empty<int>() | to_vector(), IsEmpty());
}

TEST(EmptyTest, Constructor) {
    EXPECT_THAT(Stream<int>() | to_vector(), IsEmpty());
}
