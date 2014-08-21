#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

// Arithmetic operators

TEST(AlgebraTest, Negate) {
    EXPECT_THAT(-MakeStream::from({1, 2, 3}) | to_vector(),
                ElementsAre(-1, -2, -3));
}

TEST(AlgebraTest, Addition) {
    EXPECT_THAT((MakeStream::from({1, 2, 3}) + MakeStream::from({-1, 0, 5})) | to_vector(),
                ElementsAre(0, 2, 8));
    EXPECT_THAT((MakeStream::from({1, 2, 3}) + 5) | to_vector(),
                ElementsAre(6, 7, 8));
    EXPECT_THAT((5 + MakeStream::from({1, 2, 3})) | to_vector(),
                ElementsAre(6, 7, 8));
}

TEST(AlgebraTest, Subtraction) {
    EXPECT_THAT((MakeStream::from({1, 2, 3}) - MakeStream::from({-1, 0, 5})) | to_vector(),
                ElementsAre(2, 2, -2));
    EXPECT_THAT((MakeStream::from({1, 2, 3}) - 5) | to_vector(),
                ElementsAre(-4, -3, -2));
    EXPECT_THAT((5 - MakeStream::from({1, 2, 3})) | to_vector(),
                ElementsAre(4, 3, 2));
}

TEST(AlgebraTest, Multiplication) {
    EXPECT_THAT((MakeStream::from({1, 2, 3}) * MakeStream::from({-1, 0, 5})) | to_vector(),
                ElementsAre(-1, 0, 15));
    EXPECT_THAT((MakeStream::from({1, 2, 3}) * 5) | to_vector(),
                ElementsAre(5, 10, 15));
    EXPECT_THAT((5 * MakeStream::from({1, 2, 3})) | to_vector(),
                ElementsAre(5, 10, 15));
}

TEST(AlgebraTest, Division) {
    EXPECT_THAT((MakeStream::from({5, 10, 15}) / MakeStream::from({5, 1, 3})) | to_vector(),
                ElementsAre(1, 10, 5));
    EXPECT_THAT((MakeStream::from({5, 10, 15}) / 5.0) | to_vector(),
                ElementsAre(1, 2, 3));
    EXPECT_THAT((1.0 / MakeStream::from({1, 2, 4})) | to_vector(),
                ElementsAre(1, .5, .25));
}

TEST(AlgebraTest, Modulus) {
    EXPECT_THAT((MakeStream::from({12, 13, 14}) % MakeStream::from({6, 5, 4})) | to_vector(),
                ElementsAre(0, 3, 2));
    EXPECT_THAT((MakeStream::from({5, 6, 7}) % 5) | to_vector(),
                ElementsAre(0, 1, 2));
    EXPECT_THAT((5 % MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(2, 1, 0));
}

// Comparison Operators

TEST(AlgebraTest, Equality) {
    EXPECT_THAT((MakeStream::from({5, 6}) == MakeStream::from({5, 4})) | to_vector(),
                ElementsAre(true, false));
    EXPECT_THAT((MakeStream::from({0, 3, 0}) == 3) | to_vector(),
                ElementsAre(false, true, false));
    EXPECT_THAT((3 == MakeStream::from({0, 3, 0})) | to_vector(),
                ElementsAre(false, true, false));
}

TEST(AlgebraTest, Inequality) {
    EXPECT_THAT((MakeStream::from({5, 6}) != MakeStream::from({5, 4})) | to_vector(),
                ElementsAre(false, true));
    EXPECT_THAT((MakeStream::from({0, 3, 0}) != 3) | to_vector(),
                ElementsAre(true, false, true));
    EXPECT_THAT((3 != MakeStream::from({0, 3, 0})) | to_vector(),
                ElementsAre(true, false, true));
}

TEST(AlgebraTest, LessThan) {
    EXPECT_THAT((MakeStream::from({4, 5, 6}) < MakeStream::from({6, 5, 4})) | to_vector(),
                ElementsAre(true, false, false));
    EXPECT_THAT((MakeStream::from({0, 1, 2}) < 1) | to_vector(),
                ElementsAre(true, false, false));
    EXPECT_THAT((1 < MakeStream::from({0, 1, 2})) | to_vector(),
                ElementsAre(false, false, true));
}

TEST(AlgebraTest, LessThanOrEqual) {
    EXPECT_THAT((MakeStream::from({4, 5, 6}) <= MakeStream::from({6, 5, 4})) | to_vector(),
                ElementsAre(true, true, false));
    EXPECT_THAT((MakeStream::from({0, 1, 2}) <= 1) | to_vector(),
                ElementsAre(true, true, false));
    EXPECT_THAT((1 <= MakeStream::from({0, 1, 2})) | to_vector(),
                ElementsAre(false, true, true));
}

TEST(AlgebraTest, GreaterThan) {
    EXPECT_THAT((MakeStream::from({4, 5, 6}) > MakeStream::from({6, 5, 4})) | to_vector(),
                ElementsAre(false, false, true));
    EXPECT_THAT((MakeStream::from({0, 1, 2}) > 1) | to_vector(),
                ElementsAre(false, false, true));
    EXPECT_THAT((1 > MakeStream::from({0, 1, 2})) | to_vector(),
                ElementsAre(true, false, false));
}

TEST(AlgebraTest, GreaterThanOrEqual) {
    EXPECT_THAT((MakeStream::from({4, 5, 6}) >= MakeStream::from({6, 5, 4})) | to_vector(),
                ElementsAre(false, true, true));
    EXPECT_THAT((MakeStream::from({0, 1, 2}) >= 1) | to_vector(),
                ElementsAre(false, true, true));
    EXPECT_THAT((1 >= MakeStream::from({0, 1, 2})) | to_vector(),
                ElementsAre(true, true, false));
}

// Logical operators


TEST(AlgebraTest, LogicalNot) {
    EXPECT_THAT(!MakeStream::from({true, false}) | to_vector(),
                ElementsAre(false, true));
}

TEST(AlgebraTest, LogicalAnd) {
    EXPECT_THAT((MakeStream::from({true, true, false, false}) && MakeStream::from({true, false, true, false})) | to_vector(),
                ElementsAre(true, false, false, false));
    EXPECT_THAT((MakeStream::from({true, false}) && true) | to_vector(),
                ElementsAre(true, false));
    EXPECT_THAT((false && MakeStream::from({true, false})) | to_vector(),
                ElementsAre(false, false));
}

TEST(AlgebraTest, LogicalOr) {
    EXPECT_THAT((MakeStream::from({true, true, false, false}) || MakeStream::from({true, false, true, false})) | to_vector(),
                ElementsAre(true, true, true, false));
    EXPECT_THAT((MakeStream::from({true, false}) || true) | to_vector(),
                ElementsAre(true, true));
    EXPECT_THAT((false || MakeStream::from({true, false})) | to_vector(),
                ElementsAre(true, false));
}

// Bitwise operators

TEST(AlgebraTest, BitwiseNot) {
    EXPECT_THAT(~MakeStream::from({0, 1, 2, 3}) | to_vector(),
                ElementsAre(~0, ~1, ~2, ~3));
}

TEST(AlgebraTest, BitwiseAnd) {
    EXPECT_THAT((MakeStream::from({3, 4}) & MakeStream::from({5, 6})) | to_vector(),
                ElementsAre(3 & 5, 4 & 6));
    EXPECT_THAT((MakeStream::from({3, 4, 5}) & 12) | to_vector(),
                ElementsAre(3 & 12, 4 & 12, 5 & 12));
    EXPECT_THAT((12 & MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(12 & 3, 12 & 4, 12 & 5));
}

TEST(AlgebraTest, BitwiseOr) {
    EXPECT_THAT((MakeStream::from({3, 4}) | MakeStream::from({5, 6})) | to_vector(),
                ElementsAre(3 | 5, 4 | 6));
    EXPECT_THAT((MakeStream::from({3, 4, 5}) | 12) | to_vector(),
                ElementsAre(3 | 12, 4 | 12, 5 | 12));
    EXPECT_THAT((12 | MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(12 | 3, 12 | 4, 12 | 5));
}

TEST(AlgebraTest, BitwiseXor) {
    EXPECT_THAT((MakeStream::from({3, 4}) ^ MakeStream::from({5, 6})) | to_vector(),
                ElementsAre(3 ^ 5, 4 ^ 6));
    EXPECT_THAT((MakeStream::from({3, 4, 5}) ^ 12) | to_vector(),
                ElementsAre(3 ^ 12, 4 ^ 12, 5 ^ 12));
    EXPECT_THAT((12 ^ MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(12 ^ 3, 12 ^ 4, 12 ^ 5));
}

TEST(AlgebraTest, BitShiftLeft) {
    EXPECT_THAT((MakeStream::from({3, 4}) << MakeStream::from({5, 6})) | to_vector(),
                ElementsAre(3 << 5, 4 << 6));
    EXPECT_THAT((MakeStream::from({3, 4, 5}) << 12) | to_vector(),
                ElementsAre(3 << 12, 4 << 12, 5 << 12));
    EXPECT_THAT((12 << MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(12 << 3, 12 << 4, 12 << 5));
}

TEST(AlgebraTest, BitShiftRight) {
    EXPECT_THAT((MakeStream::from({3, 4}) >> MakeStream::from({5, 6})) | to_vector(),
                ElementsAre(3 >> 5, 4 >> 6));
    EXPECT_THAT((MakeStream::from({3, 4, 5}) >> 12) | to_vector(),
                ElementsAre(3 >> 12, 4 >> 12, 5 >> 12));
    EXPECT_THAT((12 >> MakeStream::from({3, 4, 5})) | to_vector(),
                ElementsAre(12 >> 3, 12 >> 4, 12 >> 5));
}
