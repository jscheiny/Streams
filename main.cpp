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
    MakeStream::cycle(seq)
        .limit(100)
        .slice_to_end(1, 10)
        .print_to(cout);
    cout << endl;
    return 0;
}
