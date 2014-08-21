#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(CycleTest, Iterator) {
    std::vector<int> v = {1, 2, 3};
    EXPECT_THAT(
        MakeStream::cycle(std::begin(v), std::end(v)) | limit(6) | to_vector(),
        ElementsAre(1, 2, 3, 1, 2, 3));

    std::vector<int> v2 = {1, 2};
    EXPECT_THAT(MakeStream::cycle(std::begin(v2), std::end(v2), 2) | to_vector(),
                ElementsAre(1, 2, 1, 2));
}

TEST(CycleTest, Container) {
    std::vector<int> v = {1, 2, 3, 4};
    EXPECT_THAT(MakeStream::cycle(v) | limit(7) | to_vector(),
                ElementsAre(1, 2, 3, 4, 1, 2, 3));

    std::vector<int> v2 = {1};
    EXPECT_THAT(MakeStream::cycle(v2, 2) | to_vector(), ElementsAre(1, 1));
}

TEST(CycleTest, InitializerList) {
    EXPECT_THAT(MakeStream::cycle({1, 2, 3}) | limit(5) | to_vector(),
                ElementsAre(1, 2, 3, 1, 2));

    EXPECT_THAT(MakeStream::cycle({1, 2, 3}, 2) | to_vector(),
                ElementsAre(1, 2, 3, 1, 2, 3));
}

TEST(CycleTest, CycleMove) {
    std::vector<int> v = {1, 2, 3, 4};
    EXPECT_THAT(MakeStream::cycle_move(std::move(v)) | limit(5) | to_vector(),
                ElementsAre(1, 2, 3, 4, 1));

    EXPECT_THAT(MakeStream::cycle_move(std::vector<int>{1, 2, 3}, 2) | to_vector(),
                ElementsAre(1, 2, 3, 1, 2, 3));
}
