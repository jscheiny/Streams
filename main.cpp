#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>

using namespace std;

Stream<int> range(int lower, int upper) {
    return MakeStream::counter(lower).limit(upper - lower + 1);
}

int main(int argc, char const *argv[])
{

    auto seq = range(1, 5).to_vector();
    auto st = MakeStream::cycle(seq)
        .limit(10);
    for(int x : st) {
        cout << x << " ";
    }
    cout << endl;
    return 0;
}
