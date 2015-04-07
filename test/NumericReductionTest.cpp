#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

#define EXCEPTION_TEST(Operation, Exception) \
    [](){ \
        bool thrown = false; \
        try { Operation; } catch(Exception& e) { thrown = true; } \
        return thrown; \
    }()

#define EXPECT_EXCEPTION(Operation, Exception) \
    EXPECT_THAT(EXCEPTION_TEST(Operation, Exception), Eq(true));

TEST(CountTest, Default) {
    EXPECT_THAT(MakeStream::range(0, 10) | count(), Eq(10));
    EXPECT_THAT(MakeStream::empty<int>() | count(), Eq(0));
}

TEST(SumTest, NoIdentity) {
    EXPECT_THAT(MakeStream::from({3, 7}) | sum(), Eq(10));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | sum(), EmptyStreamException);
}

TEST(SumTest, Identity) {
    EXPECT_THAT(MakeStream::from({3, 7}) | sum(5), Eq(15));
    EXPECT_THAT(MakeStream::empty<int>() | sum(0), Eq(0));
}

TEST(ProductTest, NoIdentity) {
    EXPECT_THAT(MakeStream::closed_range(1, 5) | product(), Eq(120));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | product(), EmptyStreamException);
}

TEST(ProductTest, Identity) {
    EXPECT_THAT(MakeStream::from({3, 7}) | product(0), Eq(0));
    EXPECT_THAT(MakeStream::empty<int>() | product(1), Eq(1));
}

TEST(StatisticTest, Min) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | min(), Eq(-2));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | min(), EmptyStreamException);
}

TEST(StatisticTest, MinBy) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | min_by([](auto x) { return -x; }), Eq(8));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | min_by([](auto x) { return -x; }), EmptyStreamException);
}

TEST(StatisticTest, Max) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | max(), Eq(8));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | max(), EmptyStreamException);
}

TEST(StatisticTest, MaxBy) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | max_by([](auto x) { return -x; }), Eq(-2));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | max_by([](auto x) { return -x; }), EmptyStreamException);
}

TEST(StatisticTest, MinMax) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | minmax(), Eq(std::make_pair(-2, 8)));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | minmax(), EmptyStreamException);
}

TEST(StatisticTest, MinMaxBy) {
    EXPECT_THAT(MakeStream::from({3, -2, 8}) | minmax_by([](auto x) { return -x; }), Eq(std::make_pair(8, -2)));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | minmax_by([](auto x) { return -x; }), EmptyStreamException);
}
