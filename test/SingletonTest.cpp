#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(SingletonTest, Default) {
    EXPECT_THAT(make_stream::singleton(5) | to_vector(), ElementsAre(5));
}
