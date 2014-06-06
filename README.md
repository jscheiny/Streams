# C++ Streams

Streams is a C++ library that provides lazy evaluation and functional-style transformations on the data, to ease the use of C++ standard library containers and algorithms. Streams support many common functiona operations such as map, filter, and reduce, as well as various other useful operations such as various set operations (union, intersection, difference), partial sum, and adjacent difference, as well as many others. 

## Examples

### Coin flip experiment:

```cpp
using namespace stream;

int number_heads(int flips) {
    return MakeStream::coin_flips()
        .limit(1000)
        .filter()
        .count();
};

void experiment(int trials, int flips) {
    auto stats = MakeStream::generate(std::bind(number_heads, flips))
        .limit(trials)
        .reduce_by(Reducers::SummaryStats<int>());
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

int collatz_next(int value) {
    if(value % 2 == 0)
        return value / 2;
    return 3 * value + 1;
}

int collatz_sequence_length(int start) {
    return MakeStream::iterate(start, collatz_next)
        .take_while([](int x) { return x != 1; })
        .count();
}

void print_collatz(int start) {
    MakeStream::iterate(start, collatz_next)
        .take_while([](int x) { return x != 1; })
        .print_to(std::cout, " -> ");
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

auto sum_vec = (to_stream(x) + to_stream(y)).to_vector();
auto diff_vec = (to_stream(x) - to_stream(y)).to_vector();
double dot_product = (to_stream(x) * to_stream(y)).sum();
auto scaling = (to_stream(x) * 10).to_vector();
auto translating = (to_stream(x) + 3.7).to_vector();
```

### Set operations:

```cpp
std::set<int> x = /* ... */;
set::set<int> y = /* ... */;

auto to_stream = [](std::set<int>& vec) {
    return MakeStream::from(vec);
};

auto set_union = to_stream(x).union_with(to_stream(y)).to_set();
// Better than:
//   std::set<int> result;
//   std::set_union(x.begin(), x.end(), y.begin(), y.end(),
//                  inserter(result, result.end()));
auto set_intersect = to_stream(x).intersection_with(to_stream(y))
    .to_set();
auto set_diff = to_stream(x).difference_with(to_stream(y))
    .to_set();
auto set_sym_diff = to_stream(x).symmetric_difference_with(to_stream(y))
    .to_set();
```

### Adding unique ids:

```cpp
std::vector<T> objects = /* ... */;

std::vector<T> objects_with_ids = MakeStream::from(objects)
    .zip_with(MakeStream::counter(1), [](T&& object, int id) {
        object.set_id(id);
        return object;
    })
    .to_vector();
```

### Printing containers:

```cpp
MakeStream::from(container).print_to(std::cout) << std::endl;
```

### Finite differences:

```cpp
auto diff1 = stream::MakeStream::counter(1).limit(6)
    .map([](int x) { return x * x * x; })
    .adjacent_difference()
    .to_vector();
stream::MakeStream::from(diff1).print_to(std::cout) << std::endl;

auto diff2 = stream::MakeStream::from(diff1)
    .adjacent_difference()
    .to_vector();
stream::MakeStream::from(diff2).print_to(std::cout) << std::endl;

auto diff3 = stream::MakeStream::from(diff2)
    .adjacent_difference()
    .to_vector();
stream::MakeStream::from(diff3).print_to(std::cout) << std::endl;

// Output:
// 7 19 37 61 91
// 12 18 24 30
// 6 6 6
```