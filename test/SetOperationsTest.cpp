#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(SetOperationsTest, Merge) {
    EXPECT_THAT(make_stream::from({1, 1, 2, 3, 3, 4})
                    | merge_with(make_stream::from({2, 4, 4, 6}))
                    | to_vector(),
                ElementsAre(1, 1, 2, 2, 3, 3, 4, 4, 4, 6));
}

TEST(SetOperationsTest, Union) {
    EXPECT_THAT(make_stream::from({1, 2, 3, 4, 5})
                    | union_with(make_stream::from({2, 3, 4, 6, 7}))
                    | to_vector(),
                ElementsAre(1, 2, 3, 4, 5, 6, 7));
}

TEST(SetOperationsTest, Intersect) {
    EXPECT_THAT(make_stream::from({1, 2, 3, 4, 5})
                    | intersect_with(make_stream::from({2, 3, 4, 6, 7}))
                    | to_vector(),
                ElementsAre(2, 3, 4));
}

TEST(SetOperationsTest, Difference) {
    EXPECT_THAT(make_stream::from({1, 2, 3, 4, 5})
                    | difference_with(make_stream::from({2, 3, 4, 6, 7}))
                    | to_vector(),
                ElementsAre(1, 5));
}

TEST(SetOperationsTest, SymmetricDifference) {
    EXPECT_THAT(make_stream::from({1, 2, 3, 4, 5})
                    | symmetric_difference_with(make_stream::from({2, 3, 4, 6, 7}))
                    | to_vector(),
                ElementsAre(1, 5, 6, 7));
}

