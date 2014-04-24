#include "Stream.h"

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> result;
    Stream<int>::Generate([]() { static int counter; return ++counter; })
        .Filter([](int x) { return x % 2 == 0; })
        .Peek([](int x) { cout << "value = " << x << endl; })
        .Limit(4)
        .Map([](int x) { return x * 2; })
        .CopyTo(back_inserter(result));
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    return 0;
}
