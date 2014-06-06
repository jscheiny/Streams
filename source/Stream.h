#ifndef STREAM_H
#define STREAM_H

#include "StreamForward.h"
#include "StreamError.h"
#include "providers/Providers.h"
#include "Reducer.h"
#include "Utility.h"

#include <functional>
#include <type_traits>
#include <iostream>
#include <iterator>
#include <vector>
#include <random>
#include <chrono>
#include <deque>
#include <list>
#include <set>

namespace stream {

struct MakeStream {
    template<typename T>
    static Stream<RemoveRef<T>> empty();

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value);

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value, size_t times);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end,
                                                size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont,
                                                  size_t times);

    template<typename T>
    static Stream<T> cycle(std::initializer_list<T> init);

    template<typename T>
    static Stream<T> cycle(std::initializer_list<T> init, size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle_move(Container&& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle_move(Container&& cont,
                                                       size_t times);

    template<typename Generator>
    static Stream<std::result_of_t<Generator()>> generate(Generator&& generator);

    template<typename T, typename Function>
    static Stream<RemoveRef<T>> iterate(T&& initial, Function&& function);

    template<typename T>
    static Stream<RemoveRef<T>> counter(T&& start);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> counter(T&& start, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> counter(T&& start, const U& increment);

    template<typename T>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper, const U& increment);

    template<typename T>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper, const U& increment);

    template<typename T, template<typename> class Distribution,
             typename Engine=std::default_random_engine,
             typename Seed,
             typename ... GenArgs>
    static Stream<T> randoms_seeded(Seed&& seed, GenArgs&&... args);

    template<typename T, template<typename> class Distribution,
             typename Engine=std::default_random_engine,
             typename ... GenArgs>
    static Stream<T> randoms(GenArgs&&... args);

    template<typename Engine=std::default_random_engine, typename T>
    static Stream<T> uniform_random_ints(T lower, T upper);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> uniform_random_ints(T lower, T upper, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=double>
    static Stream<T> uniform_random_reals(T lower=0.0, T upper=1.0);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> uniform_random_reals(T lower, T upper, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=double>
    static Stream<T> normal_randoms(T mean=0.0, T stddev=1.0);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> normal_randoms(T mean, T stddev, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=bool>
    static Stream<T> coin_flips();

    template<typename Engine=std::default_random_engine, typename T=bool, typename Seed>
    static Stream<T> coin_flips(Seed&& seed);

    template<typename T>
    static Stream<RemoveRef<T>> singleton(T&& value);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> from(Iterator begin, Iterator end);

    template<typename Container>
    static Stream<ContainerType<Container>> from(const Container& cont);

    template<typename T>
    static Stream<T> from(T* array, std::size_t length);

    template<typename T>
    static Stream<T> from(std::initializer_list<T> init);

    template<typename Container>
    static Stream<ContainerType<Container>> from_move(Container&& cont);

private:
    static auto default_seed() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }

};

template<typename T>
class StreamImpl<T, Common> {

public:
    using ElementType = T;
    using iterator = typename provider::StreamProvider<T>::Iterator;

    iterator begin() {
        return source_->begin();
    }

    iterator end() {
        return source_->end();
    }

    StreamImpl();

    template<typename Iterator>
    StreamImpl(Iterator begin, Iterator end);

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
    Stream<std::result_of_t<Transform(T&&)>> map(Transform&& transform);

    template<typename Transform>
    Stream<detail::StreamType<std::result_of_t<Transform(T&&)>>>
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
    Stream<std::result_of_t<Subtractor(T&, T&)>>
    adjacent_difference(Subtractor&& subtract = Subtractor());

    template<typename Adder = std::plus<T>>
    Stream<T> partial_sum(Adder&& add = Adder());

    template<typename Iterator>
    Stream<T> concat(Iterator begin, Iterator end);

    Stream<T> concat(Stream<T>&& other);

    Stream<T> pad(T&& padding);

    Stream<provider::detail::GroupResult<T, 2>> pairwise();

    template<size_t N>
    Stream<provider::detail::GroupResult<T, N>> grouped();

    template<typename Other, typename Function = provider::detail::Zipper>
    Stream<std::result_of_t<Function(T&&, Other&&)>> zip_with(Stream<Other>&& other,
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
    std::result_of_t<Identity(T&&)> reduce(Identity&& identity, Accumulator&& accum);

    template<typename Accumulator>
    T reduce(Accumulator&& accumulator);

    template<typename U>
    U reduce_by(const Reducer<T, U>& reducer);

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

    T nth(size_t index);

    T operator[] (size_t index);

    std::vector<T> random_sample(size_t size);

    T random_element();

    template<typename Predicate>
    bool any(Predicate&& predicate);

    template<typename Predicate>
    bool all(Predicate&& predicate);

    template<typename Predicate>
    bool none(Predicate&& predicate);

    template<typename OutputIterator>
    OutputIterator copy_to(OutputIterator out);

    template<typename OutputIterator>
    OutputIterator move_to(OutputIterator out);

    void print_to(std::ostream& os, const char* delimiter = " ");

    std::vector<T> to_vector();

    std::list<T> to_list();

    std::deque<T> to_deque();

    template<typename Compare = std::less<T>>
    std::set<T, Compare> to_set(Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    std::multiset<T, Compare> to_multiset(Compare&& compare = Compare());

    template<typename Function>
    void for_each(Function&& function);

    inline bool occupied() const;

    inline void swap(Stream<T>& other) {
        source_.swap(other.source_);
    }

    std::ostream& print_pipeline(std::ostream& os);

    friend class MakeStream;
    template<typename, int> friend class StreamImpl;
    template<typename, typename, typename> friend class FlatMappedStreamProvider;

protected:
    inline void check_vacant(const std::string& method);

    StreamProviderPtr<T> source_;

private:
    StreamImpl(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

    template<typename Function>
    T no_identity_reduction(const std::string& name, Function&& function);

    template<typename Identity, typename Function>
    std::result_of_t<Identity(T&&)> no_identity_reduction(
        const std::string& name,
        Identity&& identity,
        Function&& function);

    struct StreamTerminator {
        StreamTerminator(StreamProviderPtr<T>& source) : source_(source) {}
        ~StreamTerminator() { source_.reset(); }

        StreamProviderPtr<T>& source_;
    };
};

template<typename T>
StreamImpl<T, Common>::StreamImpl()
    : source_(make_stream_provider<provider::Empty, T>()) {}

template<typename T>
template<typename Iterator>
StreamImpl<T, Common>::StreamImpl(Iterator begin, Iterator end)
    : source_(make_stream_provider<provider::Iterator, T, Iterator>(
        begin, end)) {}

template<typename T>
inline bool StreamImpl<T, Common>::occupied() const {
    return bool(source_);
}

template<typename T>
inline void StreamImpl<T, Common>::check_vacant(const std::string& method) {
    if(!occupied()) {
        throw VacantStreamException(method);
    }
}

template<typename T>
std::ostream& StreamImpl<T, Common>::print_pipeline(std::ostream& os) {
    int stages, sources;
    std::tie(stages, sources) = source_->print(os, 1);
    os << "Stream pipeline with " 
       << stages << " stage" << (stages == 1 ? "" : "s") << " and "
       << sources << " source" << (sources == 1 ? "" : "s") << "." << std::endl;
    return os;
}

} /* namespace stream */

#include "StreamFactories.h"
#include "StreamOperators.h"
#include "StreamTerminators.h"
#include "StreamAlgebra.h"
#include "specializations/BoolStream.h"
#include "specializations/ClassStream.h"
#include "specializations/NumberStream.h"
#include "specializations/Aggregate.h"

#endif
