#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

struct Number {
    Number(int v) : value(v) {}

    void accumulate_const() const {
        elements.push_back(value);
    }

    void accumulate() {
        elements.push_back(value);
    }

    int value = 0;

    static std::vector<int> elements;
    static void reset() {
        elements.clear();
    }
};

std::vector<int> Number::elements = {};

TEST(PeekTest, Function) {
    std::vector<int> values;
    MakeStream::range(0, 3)
        | peek([&](int x) { values.push_back(x); })
        | sum();
    EXPECT_THAT(values, ElementsAre(0, 1, 2));
}

TEST(PeekTest, MemberFunction) {
    Number::reset();
    MakeStream::from({Number(0), Number(1), Number(2)})
        | peek(&Number::accumulate)
        | for_each([](Number n) { /* do nothing */ });
    EXPECT_THAT(Number::elements, ElementsAre(0, 1, 2));

    Number::reset();
    MakeStream::from({Number(0), Number(1), Number(2)})
        | peek(&Number::accumulate_const)
        | for_each([](Number n) { /* do nothing */ });
    EXPECT_THAT(Number::elements, ElementsAre(0, 1, 2));
}
