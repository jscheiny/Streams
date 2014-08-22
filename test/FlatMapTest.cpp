#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

struct Number {
    Number(int v) : value(v) {}

    Stream<int> get_stream() const { return MakeStream::repeat(value, value); }

    int value;
};

TEST(FlatMapTest, Function) {
    EXPECT_THAT(MakeStream::range(0, 4)
                    | flat_map([](int x) { return MakeStream::repeat(x, x); })
                    | to_vector(),
                ElementsAre(1, 2, 2, 3, 3, 3));
}

TEST(FlatMapTest, MemberFunction) {
    EXPECT_THAT(MakeStream::from({Number(1), Number(3), Number(2)})
                    | flat_map(&Number::get_stream)
                    | to_vector(),
                ElementsAre(1, 3, 3, 3, 2, 2));
}
