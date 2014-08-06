#include "source/Stream.h"
#include "source/Reducers.h"

using namespace stream;
using namespace std;

int main(int argc, char const *argv[]) {
    multiset<int> s = MakeStream::from({1,2,3})
        | op::merge_with(MakeStream::from({2,3,4}));
    MakeStream::from(s) | op::print_to(cout);
}
