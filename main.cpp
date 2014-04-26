#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>


using namespace std;

Stream<int> count(int start, int increment = 1) {
    return Stream<int>::iterate(start - increment,
        [](int x) { return x + 1; });
}

Stream<int> range(int lower, int upper) {
    return Stream<int>::iterate(lower - 1, [](int x) { return x + 1; })
        .limit(upper - lower + 1);
}

int main(int argc, char const *argv[])
{
    vector<int> result = Stream<int>
        ::generate([]() { static int counter; return ++counter; })
        .skip(3)
        .filter([](int x) { return x % 2 == 0; })
        .peek([](int x) { cout << "value = " << x << endl; })
        .limit(4)
        .map([](int x) { return x * 2; })
        .concat(Stream<int>::generate([]() { return 5; }).limit(3))
        .as_vector();
    copy(result.begin(), result.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    range(1,10)
        .take_until([](int x) { return x > 5; })
        .print_to(cout);
    cout << endl;

    range(1,10)
        .drop_while([](int x) { return x < 3; })
        .print_to(cout);
    cout << endl;

    range(0, 9)
        .map([](int x) { return x % 5; })
        .distinct()
        .print_to(cout);
    cout << endl;

    range(1,5).zip(range(6,10)).for_each([](std::tuple<int, int> x) {
        cout << x << endl;
    });

    range(1, 10)
        .map([](int x) { return x * x; })
        .adjacent_difference()
        .map([](int x) { return x - 1; })
        .print_to(cout);
    cout << endl;

    range(1, 4)
        .flat_map([](int x) {
            return range(0, x);
        })
        .for_each([](int x) {
            cout << x << " ";
        });
    cout << endl;

    range(1, 5).zip(range(6, 10)).for_each(splat([](int x, int y) {
        cout << x << " -> " << y << endl;
    }));
    return 0;
}
