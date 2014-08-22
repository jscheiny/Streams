#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(ZipTest, EqualLength) {
    using Z = std::tuple<int, int>;
    EXPECT_THAT(MakeStream::range(0, 3) | zip_with(MakeStream::range(3, 6)) | to_vector(),
                ElementsAre(Z(0, 3), Z(1, 4), Z(2, 5)));
}

TEST(ZipTest, UnequalLengths) {
    using Z = std::tuple<int, int>;
    EXPECT_THAT(MakeStream::range(0, 5) | zip_with(MakeStream::range(3, 6)) | to_vector(),
                ElementsAre(Z(0, 3), Z(1, 4), Z(2, 5)));
    EXPECT_THAT(MakeStream::range(0, 3) | zip_with(MakeStream::range(3, 9)) | to_vector(),
                ElementsAre(Z(0, 3), Z(1, 4), Z(2, 5)));
}

TEST(ZipTest, MultiZips) {
    using Z3 = std::tuple<int, int, int>;
    EXPECT_THAT(MakeStream::range(0, 3)
                    | zip_with(MakeStream::range(3, 6))
                    | zip_with(MakeStream::range(6, 9))
                    | to_vector(),
                ElementsAre(Z3(0, 3, 6), Z3(1, 4, 7), Z3(2, 5, 8)));

    using Z4 = std::tuple<int, int, int, int>;
    EXPECT_THAT(MakeStream::range(0, 2)
                    | zip_with(MakeStream::range(2, 4))
                    | zip_with(MakeStream::range(4, 8))
                    | zip_with(MakeStream::range(8, 10))
                    | to_vector(),
                ElementsAre(Z4(0, 2, 4, 8) , Z4(1, 3, 5, 9)));
}

TEST(ZipTest, CustomZipper) {
    EXPECT_THAT(MakeStream::range(0, 5)
                    | zip_with(MakeStream::repeat(2), std::plus<int>())
                    | to_vector(),
                ElementsAre(2, 3, 4, 5, 6));
}
