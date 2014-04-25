#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>


using namespace std;


Stream<int> range(int lower, int upper) {
    return Stream<int>::iterate(lower - 1, [](int x) { return x + 1; })
        .limit(upper - lower + 1);
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

    range(1,10)
        .zip(range(11,20))
        .for_each([](auto x) {
            cout << x << endl;
        });



    return 0;
}
