#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>
#include <tuple>
#include <chrono>
#include <random>

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
    auto st = MakeStream::uniform_random_reals()
        .limit(100);
    st.print_pipeline(cout);
    cout << st.count() << endl;
    // st.print_to(cout, "\n");
}
