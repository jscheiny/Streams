#include "source/Stream.h"
#include "source/Reducers.h"

using namespace stream::op;
using namespace stream;
using namespace std;

int main(int argc, char const *argv[]) {
    using streams = stream::MakeStream;
    auto result = streams::uniform_random_ints(1, 10)
        | limit(1000)
        | reducers::SummaryStats<int>().reducer();
    cout << result << endl;

    int heads = streams::coin_flips()
        | limit(100)
        | filter()
        | count();
    cout << heads << endl;

    auto s = (streams::range(1, 100) * streams::counter(5))
        | filter([](int x) { return x % 2 == 0; })
        | limit(10);
    cout << s.pipeline() << endl;
    (s | print_to(cout)) << endl;

    (streams::range(1,5) * 5 | print_to(cout)) << endl;
}
