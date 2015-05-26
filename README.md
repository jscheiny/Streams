# C++ Streams with verbose types

This branch is to experiment with using streams that have encode the entire
data pipeline into the type of the stream. What do I mean by this? Previously,
the stream type simply encoded the variable type of the stream. That if you wanted
a stream of ints, you simply needed to declare a `Stream<int>` and it didn't matter
what the source of those integers was. Now if you do the following:

```cpp
auto stream = make_stream::from(some_vector) | skip(5) | filter(condition);
```

The type of stream will look something like:

```
Stream<
    provider::filter<
        provider::slice<
            provider::iterator<std::vector<int>>
        >,
        ConditionFunction
    >
>
```

I'm not yet sure if this will end up being a good idea or not, but I want to
experiment with it. Here are some pros and cons:

## Pros

*Faster runtime execution and optimization of streams.* Since the pipeline
information is all encoded in the type, we are no longer forced to use dynamic
dispatch to provide type erasure. This mean that we are no longer making virtual
function calls for every pipeline step for every element. That should speed things
up significantly. This also means that the compiler will have greater ease in
optimizing streams including increased opportunity for inlining.

*Ability to provide operators with compile time introspection.* One of the
primary reasons for this effort is to experiment with parellelized streams. Some
operators are easy to parallelize (e.g. `filter`) whereas others are difficult
(e.g. `partial_sum`). If you want to parallelize a stream that has a `partial_sum`
in the chain, you wouldn't know necessarily until runtime. But with this change
you could be told at compile time (with a nice pleasant `static_assert`).

## Cons

*More user required usage of templates and auto deduction.* Since you can no
longer just request a `Stream<int>` as a function parameter, you will pretty much
need to use templates any time you have a stream. For example:
```c++
void do_something(Stream<int>&& values) {
    ...
}
```
Must become:
```cpp
template<typename S>
void do_something(S&& values) {
    static_assert(is_stream<S>::value,
        "Must pass a stream");
    static_assert(std::is_same<stream_type<S>, int>::value,
        "Must pass a stream of ints");
    ...
}
```
And similarly, the following:
```cpp
Stream<int> get_a_stream() { ... }
```
Must become:
```cpp
auto get_a_stream() { ... }
```

*No more interoperability of streams with the same element type.* Streams are
typed by the way they were produced, not by their element type so we can no longer
do the following:
```c++
auto produce_values() {
    if (depleted) {
        return make_stream::empty<int>();
    }
    return make_stream::generate(int_generating_func);
}
```
Even though both streams returned by the function are streams of ints, they are
produced in different ways so cannot be returned from the same function.

*Longer compile times and code bloat.* The result of more templates and larger types.
