#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;
using namespace std::string_literals;

std::string int_concat(std::string&& s, int x) {
    std::stringstream ss;
    ss << s << x;
    return ss.str();
}

std::string to_string(int x) {
    return int_concat("", x);
}

#define EXCEPTION_TEST(Operation, Exception) \
    [](){ \
        bool thrown = false; \
        try { Operation; } catch(Exception& e) { thrown = true; } \
        return thrown; \
    }()

#define EXPECT_EXCEPTION(Operation, Exception) \
    EXPECT_THAT(EXCEPTION_TEST(Operation, Exception), Eq(true));

TEST(Reduce, NoIdentity) {
    EXPECT_THAT(MakeStream::range(1, 6) | reduce(std::plus<int>{}), Eq(15));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | reduce(std::plus<int>{}), EmptyStreamException);
}

TEST(Reduce, IdentityFunction) {
    EXPECT_THAT(MakeStream::range(1, 6) | reduce(to_string, int_concat), Eq("12345"s));
    EXPECT_EXCEPTION(MakeStream::empty<int>() | reduce(to_string, int_concat),
                     EmptyStreamException);
}

TEST(Reduce, SameTypeIdentity) {
    EXPECT_THAT(MakeStream::range(1, 6) | identity_reduce(1, std::multiplies<int>{}),
                Eq(120));
    EXPECT_THAT(MakeStream::empty<int>() | identity_reduce(1, std::multiplies<int>{}),
                Eq(1));
}

TEST(Reduce, DiffTypeIdentity) {
    EXPECT_THAT(MakeStream::range(1, 6) | identity_reduce(""s, int_concat),
                Eq("12345"s));
    EXPECT_THAT(MakeStream::empty<int>() | identity_reduce("BLAH"s, int_concat),
                Eq("BLAH"s));
}
