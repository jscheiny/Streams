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
    int min, max;
    tie(min, max) = MakeStream::cycle(seq)
        .limit(10)
        .minmax();
    cout << min << ", " << max << endl;
    return 0;
}
