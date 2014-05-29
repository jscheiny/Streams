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

int main(int argc, char const *argv[])
{
    Reducers::Stats<double> stats = MakeStream::normal_randoms()
        .limit(1000)
        .reduce_by(Reducers::SummaryStats<double>());
    cout << stats << endl;
}
