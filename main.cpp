#include "Stream.h"

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> x {1,2,3,4,5,6,7,8,9,10};
    vector<int> result;
    Stream<int> stream{x.begin(), x.end()};
    stream
        .filter([](int x) { return x % 2 == 0; })
        .peek([](int x) {
            static int counter = 0;
            cout << "value " << counter++ << " = " << x << endl;
        })
        .limit(4)
        .map([](int x) { return x * 2; })
        .copy_to(back_inserter(result));
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    return 0;
}
