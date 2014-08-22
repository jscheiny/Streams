#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

bool is_even_fn(int x) {
    return x % 2 == 0;
}

auto is_even_lambda = [](int x) {
    return x % 2 == 0;
};

struct Number {
    Number(int value_) : value(value_) {}
    bool is_even() { return value % 2 == 0; }
    bool is_even_const() const { return value % 2 == 0; }

    bool operator== (const Number& other) const {
        return value == other.value;
    }

    int value;
};

TEST(FilterTest, Function) {
    EXPECT_THAT(MakeStream::range(1, 5) | filter(is_even_fn) | to_vector(),
                ElementsAre(2, 4));
    EXPECT_THAT(MakeStream::range(1, 5)  | filter(is_even_lambda) | to_vector(),
                ElementsAre(2, 4));
}

TEST(FilterTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(1), Number(2), Number(3), Number(4)})
                    | filter(&Number::is_even_const)
                    | to_vector(),
                ElementsAre(Number(2), Number(4)));

    EXPECT_THAT(MakeStream::from({Number(1), Number(2), Number(3), Number(4)})
                    | filter(&Number::is_even)
                    | to_vector(),
                ElementsAre(Number(2), Number(4)));
}

TEST(FilterTest, BoolConversion) {
    Number n{1};
    Number* null = nullptr;
    EXPECT_THAT(MakeStream::from({&n, null, &n}) | filter() | to_vector(),
                ElementsAre(&n, &n));
}
