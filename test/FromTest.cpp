#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(FromTest, Iterator) {
    std::vector<int> v = {1, 2, 3};
    EXPECT_THAT(MakeStream::from(std::begin(v), std::end(v)) | to_vector(),
                ElementsAre(1, 2, 3));
}

TEST(FromTest, Container) {
    std::vector<int> v = {1, 2, 3};
    EXPECT_THAT(MakeStream::from(v) | to_vector(),
                ElementsAre(1, 2, 3));
}

TEST(FromTest, Array) {
    int array[3] = {1, 2, 3};
    EXPECT_THAT(MakeStream::from(array, 3) | to_vector(),
                ElementsAre(1, 2, 3));
}

TEST(FromTest, InitializerList) {
    EXPECT_THAT(MakeStream::from({1, 2, 3}) | to_vector(),
                ElementsAre(1, 2, 3));
}

TEST(FromTest, FromMove) {
    std::vector<int> v = {1, 2, 3};
    EXPECT_THAT(MakeStream::from_move(std::move(v)) | to_vector(),
                ElementsAre(1, 2, 3));
}
