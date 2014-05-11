#include "Stream.h"

#include "Utility.h"

#include <vector>
#include <iterator>
#include <iostream>

using namespace std;


Stream<int> range(int lower, int upper) {
    return MakeStream::counter(lower).limit(upper - lower + 1);
}

int main(int argc, char const *argv[])
{
    MakeStream::generate([]() { static int counter; return ++counter; })
        .skip(3)
        .filter([](int x) { return x % 2 == 0; })
        .peek([](int x) { cout << "value = " << x << endl; })
        .limit(4)
        .state_point()
        .map([](int x) { return x * 2; })
        .concat(MakeStream::repeat(5).limit(3))
        .concat(MakeStream::repeat(3).limit(5))
        .print_to(cout, " ");
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

    range(1,5)
        .zip_with(range(6,10))
        .for_each([](std::tuple<int, int> x) {
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

    range(1, 5)
        .zip_with(range(6, 10))
        .for_each(splat([](int x, int y) {
            cout << x << " -> " << y << endl;
        }));

    MakeStream::repeat(1)
        .partial_sum()
        .limit(10)
        .print_to(cout, " ");
    cout << endl;

    auto fn = [](int x, string s, int y) {
        cout << "x = " << x << ", s = " << s << ", y = " << y << endl;
    };

    auto fn_ = splattable(fn);
    auto tup1 = make_tuple(3, string("Hello"));
    auto tup2 = make_tuple(string("Hi"), 4);
    auto tup3 = make_tuple(2, string("What"), 4);

    fn_(+tup1, 8);
    fn_(0, +tup2);
    fn_(+tup3);

    vector<int> x { 1, 3, 8, 8, 10, 15, 23 };
    vector<int> y { 1, 6, 7, 10, 11, 12, 50, 53 };

    MakeStream::from(x)
        .merge_with(MakeStream::from(y))
        .print_to(cout, " <= ");
    cout << endl;

    x = { 1, 3, 8, 10, 15, 23 };
    y = { 1, 6, 7, 10, 11, 12, 23, 50, 53 };

    MakeStream::from(x)
        .set_union(MakeStream::from(y))
        .print_to(cout, " <= ");
    cout << endl;

    MakeStream::from(x)
        .set_intersection(MakeStream::from(y))
        .print_to(cout, " <= ");
    cout << endl;

    MakeStream::from({ 1, 1, 2, 2, 3, 3, 3, 3, 4, 5, 6, 6, 1, 1, 2, 3 })
        .adjacent_distinct()
        .grouped<3>()
        .for_each([](auto x) {
            cout << x << endl;
        });

    MakeStream::singleton(5)
        .print_to(cout);
    cout << endl;

    ((MakeStream::counter(5) + MakeStream::counter(1)) * 3)
        .limit(10)
        .print_to(cout);
    cout << endl;

    return 0;
}
