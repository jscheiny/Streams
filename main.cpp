#include "Stream.h"

#include "Reducers.h"
#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;

auto isHeads = [](bool x) { return x; };

int experiment() {
    return MakeStream::coin_flips()
        .limit(100)
        .filter(isHeads)
        .count();
}

class C {
public:
    C(int x) : x_(x) {}

    int value() const { return x_; }
    bool even() const { return x_ % 2 == 0; }

private:
    int x_;

};

int main(int argc, char const *argv[])
{
    Reducers::Stats<double> stats = MakeStream::normal_randoms()
        .limit(1000)
        .reduce_by(Reducers::SummaryStats<double>());
    cout << stats << endl;

    vector<C> x = { 1, 2, 3, 4, 5 };
    MakeStream::from(x)
        .filter(&C::even)
        .map(&C::value)
        .print_to(cout, "\n");
}
