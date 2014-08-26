#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

MATCHER_P2(IsBetween, a, b,
    std::string(negation ? "isn't" : "is") + " between " + PrintToString(a) + " and " + PrintToString(b))
    { return a <= arg && arg <= b; }

#define EXCEPTION_TEST(Operation, Exception) \
    [](){ \
        bool thrown = false; \
        try { Operation; } catch(Exception& e) { thrown = true; } \
        return thrown; \
    }()

#define EXPECT_EXCEPTION(Operation, Exception) \
    EXPECT_THAT(EXCEPTION_TEST(Operation, Exception), Eq(true));

TEST(SampleTest, RandomSample) {
    EXPECT_THAT(MakeStream::closed_range(1, 5) | random_sample(5), ElementsAre(1, 2, 3, 4, 5));
    EXPECT_THAT(MakeStream::closed_range(1, 2) | random_sample(5), ElementsAre(1, 2));
    EXPECT_THAT(MakeStream::empty<int>() | random_sample(5), IsEmpty());
    EXPECT_THAT(MakeStream::closed_range(1, 100) | random_sample(10), SizeIs(10));
}

TEST(SampleTest, RandomElement) {
    EXPECT_THAT(MakeStream::singleton(1) | random_element(), Eq(1));
    EXPECT_THAT(MakeStream::closed_range(1, 10) | random_element(), IsBetween(1, 10));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | random_element(), EmptyStreamException);
}
