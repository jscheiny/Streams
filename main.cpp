#include "source/Stream.h"
#include "source/Reducers.h"

using namespace stream;
using namespace std;

auto square = [](auto value) {
    return value * value;
};

auto sum_of_squares() {
    return op::map_(square) | op::sum();
}

int main(int argc, char const *argv[]) {
    int result = MakeStream::closed_range(1, 10)
        | sum_of_squares();
    cout << result << endl;
}
