#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(StatefulTest, StatePoint) {
    std::vector<int> result;
    MakeStream::range(0, 5)
        | peek([&](int x) { result.push_back(x); })
        | state_point()
        | filter()
        | sum();
    EXPECT_THAT(result, ElementsAre(0, 1, 2, 3, 4));
}

TEST(StatefulTest, Sort) {
    EXPECT_THAT(MakeStream::from({1, 0, 3, 5, 8, 7, 9}) | sort() | to_vector(),
                ElementsAre(0, 1, 3, 5, 7, 8, 9));
}

TEST(StatefulTest, Distinct) {
    EXPECT_THAT(MakeStream::from({1, 2, 1, 3, 3, 8, 5, 2, 8}) | distinct() | to_vector(),
                ElementsAre(1, 2, 3, 5, 8));
}
