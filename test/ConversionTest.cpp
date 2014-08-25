#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(ConversionTest, ToVectorExplicit) {
    EXPECT_THAT(MakeStream::from({1, 3, 5}) | to_vector(),
                ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToVectorImplicit) {
    std::vector<int> implicit = MakeStream::from({1, 3, 5});
    EXPECT_THAT(implicit, ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToListExplicit) {
    EXPECT_THAT(MakeStream::from({1, 3, 5}) | to_list(),
                ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToListImplicit) {
    std::list<int> implicit = MakeStream::from({1, 3, 5});
    EXPECT_THAT(implicit, ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToDequeExplicit) {
    EXPECT_THAT(MakeStream::from({1, 3, 5}) | to_deque(),
                ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToDequeImplicit) {
    std::deque<int> implicit = MakeStream::from({1, 3, 5});
    EXPECT_THAT(implicit, ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToSetExplicit) {
    EXPECT_THAT(MakeStream::from({1, 5, 3, 3, 1}) | to_set(),
                ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToSetImplicit) {
    std::set<int> implicit = MakeStream::from({1, 5, 3, 3, 1});
    EXPECT_THAT(implicit, ElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToMultiSetExplicit) {
    EXPECT_THAT(MakeStream::from({1, 5, 3, 3, 1}) | to_multiset(),
                ElementsAre(1, 1, 3, 3, 5));
}

TEST(ConversionTest, ToMultiSetImplicit) {
    std::multiset<int> implicit = MakeStream::from({1, 5, 3, 3, 1});
    EXPECT_THAT(implicit, ElementsAre(1, 1, 3, 3, 5));
}

TEST(ConversionTest, ToUnorderedSetExplicit) {
    EXPECT_THAT(MakeStream::from({1, 5, 3, 3, 1}) | to_unordered_set(),
                UnorderedElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToUnorderedSetImplicit) {
    std::unordered_set<int> implicit = MakeStream::from({1, 5, 3, 3, 1});
    EXPECT_THAT(implicit, UnorderedElementsAre(1, 3, 5));
}

TEST(ConversionTest, ToUnorderedMultiSetExplicit) {
    EXPECT_THAT(MakeStream::from({1, 5, 3, 3, 1}) | to_unordered_multiset(),
                UnorderedElementsAre(1, 1, 3, 3, 5));
}

TEST(ConversionTest, ToUnorderedMultimultisetImplicit) {
    std::unordered_multiset<int> implicit = MakeStream::from({1, 5, 3, 3, 1});
    EXPECT_THAT(implicit, UnorderedElementsAre(1, 1, 3, 3, 5));
}

