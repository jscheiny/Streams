#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>

using namespace std;

Stream<int> range(int lower, int upper) {
    return MakeStream::counter(lower).limit(upper - lower + 1);
}

Stream<double> random(int limit) {
    static unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    static default_random_engine generator(seed);
    static uniform_real_distribution<double> distro(0, 1);
    return MakeStream::generate([]() {
        return distro(generator);
    }).limit(limit);
}

Stream<int> randints(int lower, int upper, int limit) {
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    return MakeStream::generate([=]() {
        static default_random_engine generator(seed);
        static uniform_int_distribution<int> distro(lower, upper);
        return distro(generator);
    }).limit(limit);
}

std::vector<int> sorted_random() {
    cout << "Vec =";
    auto res = randints(0, 10, 10)
        .sort()
        .distinct()
        .peek([](int x) { cout << " " << x; })
        .to_vector();
    cout << endl;
    return res;
}

int main(int argc, char const *argv[])
{
    auto vec = MakeStream::from(sorted_random())
        .set_difference(MakeStream::from(sorted_random()))
        .to_vector();
    cout << "Sym diff: ";
    for(auto i : vec) {
        cout << i << " ";
    }
    cout << endl << endl;

    vec = MakeStream::from(sorted_random())
        .set_symmetric_difference(MakeStream::from(sorted_random()))
        .to_vector();
    cout << "Sym diff: ";
    for(auto i : vec) {
        cout << i << " ";
    }
    cout << endl << endl;

    vec = MakeStream::from(sorted_random())
        .set_union(MakeStream::from(sorted_random()))
        .to_vector();
    cout << "Union: ";
    for(auto i : vec) {
        cout << i << " ";
    }
    cout << endl << endl;

    vec = MakeStream::from(sorted_random())
        .merge_with(MakeStream::from(sorted_random()))
        .to_vector();
    cout << "Merge: ";
    for(auto i : vec) {
        cout << i << " ";
    }
    cout << endl << endl;

    vec = MakeStream::from(sorted_random())
        .set_intersection(MakeStream::from(sorted_random()))
        .to_vector();
    cout << "Intersection: ";
    for(auto i : vec) {
        cout << i << " ";
    }
    cout << endl << endl;
    return 0;
}
