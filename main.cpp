#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>
#include <iomanip>

#include "source/Stream.h"
#include "source/Reducers.h"

using namespace std;

auto square = [](auto x) { return x * x; };
auto lessThan = [](auto x) {
    return [x](auto y) {
        return x < y;
    };
};

class C {
public:
    C(int x) : x_(x) {}

    int value() const { return x_; }
    bool even() const { return x_ % 2 == 0; }

private:
    int x_;

};

int main(int argc, char const *argv[]) {
    auto coin_flips = []() {
        return stream::MakeStream::coin_flips<std::default_random_engine, int>();
    };

    auto experiment = [coin_flips](int length) {
        return (coin_flips() * coin_flips())
            .limit(length)
            .filter()
            .count();
    };

    cout << stream::MakeStream::generate(bind(experiment, 1000))
        .limit(1000)
        .reduce_by(Reducers::SummaryStats<size_t>()) << endl;

    auto square = [](auto&& x) { return x * x; };

    (stream::MakeStream::counter(0).map(square).adjacent_difference().limit(10) * coin_flips()).print_to(cout);
    cout << endl;
}
