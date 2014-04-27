# C++ Streams

The purpose of this project is to provide sequential streams similar to Java 8
Streams to the C++ language. These streams are designed to work well on their
own and also integrate well into existing C++ STL frameworks.

# Examples

# Creation from containers

Streams are easy to create from existing containers in the standard C++ way.

```c++
std::vector<int> x = {1,2,3,4,5};
Stream<int> st(x.begin(), x.end());
```

This stream will now read from the vector x and hold its data.

# Saving streams

To print out a stream simply use the `print_to` method:

```c++
st.print_to(std::cout); // 1 2 3 4 5
```

To save to another container, use the `copy_to` method which works almost 
identically to `std::copy`.

```c++
std::vector<int> result;
st.copy_to(std::back_inserter(result));
```

Note that the `print_to` method is simply a convenience for `copy_to` in the
following way:

```c++
st.copy_to(std::ostream_iterator<int>(std::cout, " "));
// equivalent to st.print_to(std::cout, " ");
``` 