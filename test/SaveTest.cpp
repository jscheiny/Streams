#include <Stream.h>

#include <gmock/gmock.h>

using namespace testing;
using namespace stream;
using namespace stream::op;

TEST(SaveTest, CopyTo) {
    std::vector<int> destination;
    MakeStream::range(1,6) | copy_to(std::back_inserter(destination));
    EXPECT_THAT(destination, ElementsAre(1, 2, 3, 4, 5));
}

TEST(SaveTest, MoveTo) {
    std::vector<std::unique_ptr<int>> destination;
    MakeStream::range(1, 3)
        | map_([](int x) { return std::make_unique<int>(x); })
        | move_to(std::back_inserter(destination));
    EXPECT_THAT(destination, ElementsAre(Pointee(1), Pointee(2)));
}

TEST(SaveTest, PrintTo) {
    std::stringstream result;
    (MakeStream::closed_range(1, 3) | print_to(result, "|")) << "END";
    EXPECT_THAT(result.str(), Eq("1|2|3|END"));
}
