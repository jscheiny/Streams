#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(EmptyTest, make_stream) {
    EXPECT_THAT(make_stream::empty<int>() | to_vector(), IsEmpty());
}
