#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

bool less_than_5_fn(int x) {
    return x < 5;
}

auto less_than_5_lambda = [](int x) {
    return x < 5;
};

struct Number {
    Number(int value_) : value(value_) {}
    bool less_than_5() { return value < 5; }
    bool less_than_5_const() const { return value < 5; }

    bool operator== (const Number& other) const {
        return value == other.value;
    }

    int value;
};

TEST(TakeWhileTest, Function) {
    EXPECT_THAT(MakeStream::range(2, 8) | take_while(less_than_5_fn) | to_vector(),
                ElementsAre(2, 3, 4));
    EXPECT_THAT(MakeStream::range(2, 8)  | take_while(less_than_5_lambda) | to_vector(),
                ElementsAre(2, 3, 4));
}

TEST(DropWhileTest, Function) {
    EXPECT_THAT(MakeStream::range(2, 8) | drop_while(less_than_5_fn) | to_vector(),
                ElementsAre(5, 6, 7));
    EXPECT_THAT(MakeStream::range(2, 8)  | drop_while(less_than_5_lambda) | to_vector(),
                ElementsAre(5, 6, 7));
}

TEST(TakeWhileTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(3), Number(4), Number(5), Number(6)})
                    | take_while(&Number::less_than_5)
                    | to_vector(),
                ElementsAre(Number(3), Number(4)));

    EXPECT_THAT(MakeStream::from({Number(3), Number(4), Number(5), Number(6)})
                    | take_while(&Number::less_than_5_const)
                    | to_vector(),
                ElementsAre(Number(3), Number(4)));
}

TEST(DropWhileTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(3), Number(4), Number(5), Number(6)})
                    | drop_while(&Number::less_than_5)
                    | to_vector(),
                ElementsAre(Number(5), Number(6)));

    EXPECT_THAT(MakeStream::from({Number(3), Number(4), Number(5), Number(6)})
                    | drop_while(&Number::less_than_5_const)
                    | to_vector(),
                ElementsAre(Number(5), Number(6)));
}

TEST(TakeWhileTest, BoolConversion) {
    Number n{1};
    Number* null = nullptr;
    EXPECT_THAT(MakeStream::from({&n, &n, null, &n}) | take_while() | to_vector(),
                ElementsAre(&n, &n));
}

TEST(DropWhileTest, BoolConversion) {
    Number n{1};
    Number* null = nullptr;
    EXPECT_THAT(MakeStream::from({&n, &n, null, &n}) | drop_while() | to_vector(),
                ElementsAre(null, &n));
}
