#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(AdjacentDistinctTest, Default) {
    EXPECT_THAT(MakeStream::from({1, 1, 1, 2, 2, 2, 1, 1, 1, 3})
                    | adjacent_distinct()
                    | to_vector(),
                ElementsAre(1, 2, 1, 3));
}

TEST(AdjacentDistinctTest, WithEquals) {
    EXPECT_THAT(MakeStream::from({1, 2, 5, 10, 0, 8, 12, -1, 3, 5})
                    | adjacent_distinct([](int x, int y) { return x < y; })
                    | to_vector(),
                ElementsAre(1, 0, -1));
}
