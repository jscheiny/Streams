#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(GroupTest, CompilePairs) {
    using G = std::pair<int, int>;
    EXPECT_THAT(MakeStream::range(0, 6) | pairwise() | to_vector(),
                ElementsAre(G{0, 1}, G{2, 3}, G{4, 5}));
    EXPECT_THAT(MakeStream::range(0, 6) | group<2>() | to_vector(),
                ElementsAre(G{0, 1}, G{2, 3}, G{4, 5}));
    EXPECT_THAT(MakeStream::range(0, 5) | pairwise() | to_vector(),
                ElementsAre(G{0, 1}, G{2, 3}));
}

TEST(GroupTest, CompileTriples) {
    using G = std::tuple<int, int, int>;
    EXPECT_THAT(MakeStream::range(0, 6) | group<3>() | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
    EXPECT_THAT(MakeStream::range(0, 7) | group<3>() | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
    EXPECT_THAT(MakeStream::range(0, 8) | group<3>() | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
}


TEST(GroupTest, CompileFourTuples) {
    using G = std::tuple<int, int, int, int>;
    EXPECT_THAT(MakeStream::range(0, 8) | group<4>() | to_vector(),
                ElementsAre(G{0, 1, 2, 3}, G{4, 5, 6, 7}));
}

TEST(GroupTest, DynamicTriples) {
    using G = std::vector<int>;
    EXPECT_THAT(MakeStream::range(0, 6) | group(3) | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
    EXPECT_THAT(MakeStream::range(0, 7) | group(3) | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
    EXPECT_THAT(MakeStream::range(0, 8) | group(3) | to_vector(),
                ElementsAre(G{0, 1, 2}, G{3, 4, 5}));
}


TEST(GroupTest, DynamicFourTuples) {
    using G = std::vector<int>;
    EXPECT_THAT(MakeStream::range(0, 8) | group(4) | to_vector(),
                ElementsAre(G{0, 1, 2, 3}, G{4, 5, 6, 7}));
}
