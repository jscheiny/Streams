#ifndef STREAM_H
#define STREAM_H

#include "StreamError.h"
#include "Providers.h"
#include "Utility.h"

#include <functional>
#include <type_traits>
#include <iostream>
#include <iterator>
#include <vector>
#include <list>

struct MakeStream {
    template<typename T>
    static Stream<RemoveRef<T>> empty();

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value);

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value, size_t times);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(Container&& cont);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end,
                                                size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont,
                                                  size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(Container&& cont,
                                                  size_t times);

    template<typename Generator>
    static Stream<ReturnType<Generator>> generate(Generator&& generator);

    template<typename T, typename Function>
    static Stream<RemoveRef<T>> iterate(T&& initial, Function&& function);

    template<typename T>
    static Stream<RemoveRef<T>> counter(T&& start);

    template<typename T>
    static Stream<RemoveRef<T>> counter(T&& start, T&& increment);

    template<typename T>
    static Stream<RemoveRef<T>> counter(T&& start, const T& increment);

    template<typename T>
    static Stream<RemoveRef<T>> singleton(T&& value);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> from(Iterator begin, Iterator end);

    template<typename Container>
    static Stream<ContainerType<Container>> from(const Container& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> from(Container&& cont);

    template<typename T>
    static Stream<T> from(T* array, std::size_t length);

    template<typename T>
    static Stream<T> from(std::initializer_list<T> init);
};

template<typename T>
class Stream {

public:
    using ElementType = T;
    using iterator = typename StreamProvider<T>::Iterator;

    iterator begin() {
        return source_->begin();
    }

    iterator end() {
        return source_->end();
    }

    Stream();

    template<typename Iterator>
    Stream(Iterator begin, Iterator end);

    template<typename Container>
    Stream(const Container& cont);

    /*** Intermediate Stream Operations ***/

    template<typename Predicate>
    Stream<T> filter(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> take_while(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> drop_while(Predicate&& predicate);

    Stream<T> slice(size_t start, size_t end, size_t increment = 1);

    Stream<T> slice_to_end(size_t start, size_t increment);

    template<typename Action>
    Stream<T> peek(Action&& action);

    template<typename Transform>
    Stream<ReturnType<Transform, T&>> map(Transform&& transform);

    template<typename Transform>
    Stream<StreamType<ReturnType<Transform, T&>>>
    flat_map(Transform&& transform);

    Stream<T> limit(std::size_t length);

    Stream<T> skip(std::size_t amount);

    template<typename Equal = std::equal_to<T>>
    Stream<T> adjacent_distinct(Equal&& equal = Equal());

    template<typename Compare = std::less<T>>
    Stream<T> sort(Compare&& comparator = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> distinct(Compare&& comparator = Compare());

    // Is there a better name for this? force_state()?
    Stream<T> state_point();

    template<typename Subtractor = std::minus<void>>
    Stream<ReturnType<Subtractor, T&, T&>>
    adjacent_difference(Subtractor&& subtract = Subtractor());

    template<typename Adder = std::plus<T>>
    Stream<T> partial_sum(Adder&& add = Adder());

    template<typename Iterator>
    Stream<T> concat(Iterator begin, Iterator end);

    Stream<T> concat(Stream<T>&& other);

    Stream<T> pad(T&& padding);

    Stream<GroupResult<T, 2>> pairwise();

    template<size_t N>
    Stream<GroupResult<T, N>> grouped();

    template<typename Other, typename Function = Zipper>
    Stream<ReturnType<Function, T&, Other&>> zip_with(Stream<Other>&& other,
        Function&& zipper = Function());

    template<typename Compare = std::less<T>>
    Stream<T> merge_with(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> union_with(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> intersection_with(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> difference_with(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> symmetric_difference_with(Stream<T>&& other, Compare&& compare = Compare());

    /*** Terminal Stream operations ***/

    size_t count();

    template<typename U, typename Accumulator>
    U reduce(const U& identity, Accumulator&& accumulator);

    template<typename Identity, typename Accumulator>
    ReturnType<Identity, T&> reduce(Identity&& identity, Accumulator&& accum);

    template<typename Accumulator>
    T reduce(Accumulator&& accumulator);

    T sum();

    T sum(const T& identity);

    T product();

    T product(const T& identity);

    template<typename Compare = std::less<T>>
    T max(Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    T min(Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    std::pair<T, T> minmax(Compare&& compare = Compare());

    T first();

    T last();

    template<typename Predicate>
    bool any(Predicate&& predicate);

    template<typename Predicate>
    bool all(Predicate&& predicate);

    template<typename Predicate>
    bool none(Predicate&& predicate);

    template<typename OutputIterator>
    void copy_to(OutputIterator out);

    template<typename OutputIterator>
    void move_to(OutputIterator out);

    void print_to(std::ostream& os, const char* delimiter = " ");

    std::vector<T> to_vector();

    std::list<T> to_list();

    template<typename Function>
    void for_each(Function&& function);

    inline bool occupied() const;

    inline void swap(Stream<T>& other) {
        source_.swap(other.source_);
    }

    std::ostream& print_pipeline(std::ostream& os);

    friend class MakeStream;

    template<typename>
    friend class Stream;

    template<typename, typename, typename>
    friend class FlatMappedStreamProvider;

private:
    Stream(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

    StreamProviderPtr<T> source_;

    inline void check_vacant(const std::string& method);

    template<typename Function>
    T no_identity_reduction(const std::string& name, Function&& function);

    template<typename Identity, typename Function>
    ReturnType<Identity, T&> no_identity_reduction(
        const std::string& name,
        Identity&& identity,
        Function&& function);
};

template<typename T>
Stream<T>::Stream()
    : source_(make_stream_provider<EmptyStreamProvider, T>()) {}

template<typename T>
template<typename Iterator>
Stream<T>::Stream(Iterator begin, Iterator end)
    : source_(make_stream_provider<IteratorStreamProvider, T, Iterator>(
        begin, end)) {}

template<typename T>
inline bool Stream<T>::occupied() const {
    return bool(source_);
}

template<typename T>
inline void Stream<T>::check_vacant(const std::string& method) {
    if(!occupied()) {
        throw VacantStreamException(method);
    }
}

template<typename T>
std::ostream& Stream<T>::print_pipeline(std::ostream& os) {
    int stages, sources;
    std::tie(stages, sources) = source_->print(os, 1);
    os << "Stream pipeline with " 
       << stages << " stage" << (stages == 1 ? "" : "s") << " and "
       << sources << " source" << (sources == 1 ? "" : "s") << "." << std::endl;
    return os;
}

#include "StreamFactoriesImpl.h"
#include "StreamOperatorsImpl.h"
#include "StreamTerminatorsImpl.h"
#include "StreamAlgebra.h"

#endif
