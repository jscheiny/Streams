# C++ Streams

Streams is a C++ library that provides lazy evaluation and functional-style transformations on the data, to ease the use of C++ standard library containers and algorithms. Streams support many common functional operations such as map, filter, and reduce, as well as various other useful operations such as various set operations (union, intersection, difference), partial sum, and adjacent difference, as well as many others.

To use, simply `#include "Stream.h"`, and compile using a C++14 compatible compiler. All streams classes/functions can be found in the `stream` namespace.

Links:

* [Documentation](http://jscheiny.github.io/Streams)
* [Github](http://github.com/jscheiny/Streams)
* [Contact](http://scheinerman.net/jonah)

C++ Streams are distributed under the MIT open source license.
Copyright (c) 2014 by Jonah Scheinerman

## Examples

### Coin flip experiment:

```cpp
using namespace stream;
using namespace stream::op;

int number_heads(int flips) {
    return MakeStream::coin_flips()
        | limit(flips)
        | filter()
        | count();
};

void experiment(int trials, int flips) {
    auto stats = MakeStream::generate(std::bind(number_heads, flips))
        | limit(trials)
        | reducers::SummaryStats<int>().reducer();
    std::cout << stats << std::endl;
}

// Example output for experiment(1000, 1000):
// N=1000, u=499.812, s=252.763, min=452, max=549
```

### Investigating the Collatz conjecture:

```cpp
#include "Stream.h"
#include <iostream>

using namespace stream;
using namespace stream::op;

int collatz_next(int value) {
    if(value % 2 == 0)
        return value / 2;
    return 3 * value + 1;
}

int collatz_sequence_length(int start) {
    return MakeStream::iterate(start, collatz_next)
        | take_while([](int x) { return x != 1; })
        | count();
}

void print_collatz(int start) {
    MakeStream::iterate(start, collatz_next)
        | take_while([](int x) { return x != 1; })
        | print_to(std::cout, " -> ");
    std::cout << "1" << std::endl;
}

int main(int argc, char const *argv[]) {
    print_collatz(24);
}

// print_collatz(10):
// 24 -> 12 -> 6 -> 3 -> 10 -> 5 -> 16 -> 8 -> 4 -> 2 -> 1
```

### Vector operations:

```cpp
std::vector<double> x = /* ... */;
std::vector<double> y = /* ... */;

auto to_stream = [](std::vector<double>& vec) {
    return MakeStream::from(vec);
};

std::vector<double> sum_vec = to_stream(x) + to_stream(y);
std::vector<double> diff_vec = to_stream(x) - to_stream(y);
double dot_product = (to_stream(x) * to_stream(y)) | sum();
std::vector<double> scaling = to_stream(x) * 10;
std::vector<double> translating = to_stream(x) + 3.7;
```

### Set operations:

```cpp
std::set<int> x = /* ... */;
set::set<int> y = /* ... */;

auto to_stream = [](std::set<int>& vec) {
    return MakeStream::from(vec);
};

std::set<int> set_union = to_stream(x) | union_with(to_stream(y));
// Better than:
//   std::set<int> result;
//   std::set_union(x.begin(), x.end(), y.begin(), y.end(),
//                  inserter(result, result.end()));
std::set<int> set_intersect = to_stream(x)
    | intersection_with(to_stream(y));
std::set<int> set_diff = to_stream(x)
    | difference_with(to_stream(y));
std::set<int> set_sym_diff = to_stream(x)
    | symmetric_difference_with(to_stream(y));
```

### Adding unique ids:

```cpp
std::vector<T> objects = /* ... */;

std::vector<T> objects_with_ids = MakeStream::from(objects)
    | zip_with(MakeStream::counter(1), [](T&& object, int id) {
        object.set_id(id);
        return object;
    });
```

### Printing containers:

```cpp
(MakeStream::from(container) | print_to(std::cout)) << std::endl;
```

### Operator composition:

```cpp
auto square = map_([](auto&& x) { return x * x; });
(MakeStream::range(1, 6) | square | print_to(std::cout)) << std::endl; // 1 4 9 16 25

auto square_and_sum = square | sum();
int result = MakeStream::range(1, 4) | square_and_sum; // 14

auto every_nth = [](int n) {
    return zip_with(MakeStream::counter(0))
         | filter([=](const auto& tup) { return std::get<1>(tup) % n == 0; })
         | map_([](auto&& tup) { return std::get<0>(tup); });
};

MakeStream::from({1, 3, 8, 4, 7}) | every_nth(2) | print_to(std::cout); // 1 8 7
```
