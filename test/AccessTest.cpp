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

TEST(FirstTest, Default) {
    EXPECT_THAT(MakeStream::counter(0) | first(), Eq(0));
}

TEST(FirstTest, EmptyStream) {
    EXPECT_EXCEPTION(MakeStream::empty<int>() | first(), EmptyStreamException);
}

TEST(LastTest, Default) {
    EXPECT_THAT(MakeStream::range(0, 10) | last(), Eq(9));
}

TEST(LastTest, EmptyStream) {
    EXPECT_EXCEPTION(MakeStream::empty<int>() | last(), EmptyStreamException);
}

TEST(NthTest, Default) {
    EXPECT_THAT(MakeStream::counter(0) | nth(5), Eq(5));
}

TEST(NthTest, EmptyStream) {
    EXPECT_EXCEPTION(MakeStream::range(0, 3) | nth(5), EmptyStreamException);
    EXPECT_EXCEPTION(MakeStream::empty<int>() | nth(5), EmptyStreamException);
}
