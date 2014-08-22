#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(AdjacentDifferenceTest, Default) {
    EXPECT_THAT(MakeStream::closed_range(1, 6)
                    | partial_sum()
                    | to_vector(),
                ElementsAre(1, 3, 6, 10, 15, 21));
}

TEST(AdjacentDifferenceTest, WithSubtraction) {
    EXPECT_THAT(MakeStream::closed_range(1, 6)
                    | partial_sum(std::multiplies<int>())
                    | to_vector(),
                ElementsAre(1, 2, 6, 24, 120, 720));
}
