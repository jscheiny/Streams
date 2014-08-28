#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(RecurrenceTest, Order1) {
    auto addone = [](int x) { return x + 1; };
    EXPECT_THAT(MakeStream::recurrence(addone, 1) | limit(5) | to_vector(),
                ElementsAre(1, 2, 3, 4, 5));

}

TEST(RecurrenceTest, Iterate) {
    auto addone = [](int x) { return x + 1; };
    EXPECT_THAT(MakeStream::iterate(1, addone) | limit(5) | to_vector(),
                ElementsAre(1, 2, 3, 4, 5));
}

TEST(RecurrenceTest, Order2) {
    EXPECT_THAT(MakeStream::recurrence(std::plus<int>{}, 0, 1) | limit(8) | to_vector(),
                ElementsAre(0, 1, 1, 2, 3, 5, 8, 13));
}

TEST(RecurrenceTest, Order3) {
    auto F = [](int a3, int a2, int a1) {
        return 2 * a3 + a2 + a1;
    };
    EXPECT_THAT(MakeStream::recurrence(F, 1, 2, 3) | limit(8) | to_vector(),
                ElementsAre(1, 2, 3, 7, 14, 27, 55, 110));
}
