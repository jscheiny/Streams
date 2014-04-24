#include "Stream.h"

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

Stream<int> range(int lower, int upper) {
    return Stream<int>::generate([lower]() {
        static int counter = lower; return ++counter;
    }).limit(upper - lower + 1);
}

int main(int argc, char const *argv[])
{
    vector<int> result;
    Stream<int>::generate([]() { static int counter; return ++counter; })
        .skip(3)
        .filter([](int x) { return x % 2 == 0; })
        .peek([](int x) { cout << "value = " << x << endl; })
        .limit(4)
        .map([](int x) { return x * 2; })
        .extend(Stream<int>::generate([]() { return 5; }).limit(3))
        .copy_to(back_inserter(result));
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    vector<tuple<int, int, int>> result2;
    range(1, 5).zip(range(6, 10)).zip(range(11, 15))
        .copy_to(back_inserter(result2));
    for(auto elem : result2) {
        cout << "(" << get<0>(elem) << ", " << get<1>(elem) << ", " <<  get<2>(elem) << ")" << endl;
    }

    return 0;
}
