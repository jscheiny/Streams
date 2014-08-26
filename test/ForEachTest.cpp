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

TEST(ForEachTest, Default) {
    std::vector<int> result;
    MakeStream::range(1, 5) | for_each([&](int x) { result.push_back(x); });
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 4));
}

template<typename T>
struct Accumulator {
    void operator() (T&& t) {
        result.emplace_back(std::forward<T>(t));
    }
    std::vector<T> result;
};

TEST(ForEachTest, Result) {
    Accumulator<int> accum = MakeStream::range(1, 5) | for_each(Accumulator<int>{});
    EXPECT_THAT(accum.result, ElementsAre(1, 2, 3, 4));
}
