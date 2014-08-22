#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

bool is_even(int x) {
    return x % 2 ==0;
}

struct Number {
    Number(int v) : value(v) {}

    bool is_even() const { return value % 2 == 0; }

    int value;
};

// Any

TEST(AnyTest, Function) {
    EXPECT_THAT(MakeStream::from({1, 3, 4}) | any(is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({1, 3, 5}) | any(is_even), Eq(false));
    EXPECT_THAT(MakeStream::empty<int>() | any(is_even), Eq(false));
}

TEST(AnyTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(1), Number(2)}) | any(&Number::is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({Number(1), Number(3)}) | any(&Number::is_even), Eq(false));
}

TEST(AnyTest, BoolConversion) {
    EXPECT_THAT(MakeStream::from({0, 3, 0}) | any(), Eq(true));
    EXPECT_THAT(MakeStream::from({false, false}) | any(), Eq(false));
}

// All

TEST(AllTest, Function) {
    EXPECT_THAT(MakeStream::from({0, 2, 4}) | all(is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({2, 3, 4}) | all(is_even), Eq(false));
    EXPECT_THAT(MakeStream::empty<int>() | all(is_even), Eq(true));
}

TEST(AllTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(0), Number(2)}) | all(&Number::is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({Number(0), Number(1)}) | all(&Number::is_even), Eq(false));
}

TEST(AllTest, BoolConversion) {
    EXPECT_THAT(MakeStream::from({3, 3, 3}) | all(), Eq(true));
    EXPECT_THAT(MakeStream::from({false, true}) | all(), Eq(false));
}

// None

TEST(NoneTest, Function) {
    EXPECT_THAT(MakeStream::from({1, 3, 5}) | none(is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({2, 3, 5}) | none(is_even), Eq(false));
    EXPECT_THAT(MakeStream::empty<int>() | none(is_even), Eq(true));
}

TEST(NoneTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(1), Number(3)}) | none(&Number::is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({Number(0), Number(1)}) | none(&Number::is_even), Eq(false));
}

TEST(NoneTest, BoolConversion) {
    EXPECT_THAT(MakeStream::from({0, 0, 0}) | none(), Eq(true));
    EXPECT_THAT(MakeStream::from({false, true}) | none(), Eq(false));
}

// Not all

TEST(NotAllTest, Function) {
    EXPECT_THAT(MakeStream::from({2, 3, 4}) | not_all(is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({0, 2, 4}) | not_all(is_even), Eq(false));
    EXPECT_THAT(MakeStream::empty<int>() | not_all(is_even), Eq(false));
}

TEST(NotAllTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(0), Number(1)}) | not_all(&Number::is_even), Eq(true));
    EXPECT_THAT(MakeStream::from({Number(0), Number(2)}) | not_all(&Number::is_even), Eq(false));
}

TEST(NotAllTest, BoolConversion) {
    EXPECT_THAT(MakeStream::from({0, 0, 1}) | not_all(), Eq(true));
    EXPECT_THAT(MakeStream::from({true, true}) | not_all(), Eq(false));
}
