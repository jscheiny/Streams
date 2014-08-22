#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(ConcatTest, Stream) {
    EXPECT_THAT(MakeStream::from({1, 2, 3}) | concat(MakeStream::from({6, 7, 8})) | to_vector(),
                ElementsAre(1, 2, 3, 6, 7, 8));
}

TEST(ConcatTest, Iterator) {
    std::vector<int> v = {6, 7, 8};
    EXPECT_THAT(MakeStream::from({1, 2, 3}) | concat(std::begin(v), std::end(v)) | to_vector(),
                ElementsAre(1, 2, 3, 6, 7, 8));
}
