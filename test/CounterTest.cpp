#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(CounterTest, Start) {
    EXPECT_THAT(MakeStream::counter(-5) | limit(5) | to_vector(),
                ElementsAre(-5, -4, -3, -2, -1));

    EXPECT_THAT(MakeStream::counter(3.0) | limit(5) | to_vector(),
                ElementsAre(3.0, 4.0, 5.0, 6.0, 7.0));
}

TEST(CounterTest, Increment) {
    EXPECT_THAT(MakeStream::counter(0, 5) | limit(5) | to_vector(),
                ElementsAre(0, 5, 10, 15, 20));


    EXPECT_THAT(MakeStream::counter(3.0, 1.5) | limit(5) | to_vector(),
                ElementsAre(3.0, 4.5, 6, 7.5, 9));

    int increment = 5;
    const int& inc_ref = increment;
    EXPECT_THAT(MakeStream::counter(0, inc_ref) | limit(5) | to_vector(),
                ElementsAre(0, 5, 10, 15, 20));
}
