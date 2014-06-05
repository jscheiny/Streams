#include "source/Stream.h"
#include "source/Reducers.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>
#include <iomanip>

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
    int heads = MakeStream::coin_flips()
        .limit(1000)
        .filter()
        .count();
    cout << heads << endl;

    C a(1);
    C b(2);
    C c(3);
    vector<C*> cs = {&a, &b, nullptr, &c};
    MakeStream::from(cs)
        .filter()
        .map(&C::value)
        .print_to(cout);
    cout << endl;

    Stream<double> s;
    cout << s.product() << endl;
}
