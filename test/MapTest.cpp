#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

int square(int num) {
    return num * num;
}

struct Number {
    Number(int v) : value(v) {}

    int get_value_const() const { return value; }
    int get_value() { return value; }

    bool operator== (const Number& other) const {
        return value == other.value;
    }

    int value = 0;
};

TEST(MapTest, Function) {
    EXPECT_THAT(MakeStream::range(0, 5) | map_(square) | to_vector(),
                ElementsAre(0, 1, 4, 9, 16));
}

TEST(MapTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(1), Number(2), Number(3)})
                    | map_(&Number::get_value)
                    | to_vector(),
                ElementsAre(1, 2, 3));

    EXPECT_THAT(MakeStream::from({Number(1), Number(2), Number(3)})
                    | map_(&Number::get_value_const)
                    | to_vector(),
                ElementsAre(1, 2, 3));
}
