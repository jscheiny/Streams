#include "Stream.h"

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

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
    return 0;
}
