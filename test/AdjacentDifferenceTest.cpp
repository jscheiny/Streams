#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(AdjacentDifferenceTest, Default) {
    EXPECT_THAT(MakeStream::range(0, 10, 2)
                    | adjacent_difference()
                    | to_vector(),
                ElementsAre(2, 2, 2, 2));
}

TEST(AdjacentDifferenceTest, WithSubtraction) {
    EXPECT_THAT(MakeStream::from({1, 3, -2, 5, 7})
                    | adjacent_difference(std::plus<int>())
                    | to_vector(),
                ElementsAre(4, 1, 3, 12));
}
