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
    static Stream<RemoveRef<T>> repeat(T&& value);

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value, size_t times);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(Container&& cont);

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

    Stream();

    template<typename Iterator>
    Stream(Iterator begin, Iterator end);

    template<typename Container>
    Stream(const Container& cont);

    template<typename Predicate>
    Stream<T> filter(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> take_while(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> drop_while(Predicate&& predicate);

    Stream<T> slice(size_t start, size_t end, size_t increment = 1);

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
    Stream<T> set_union(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> set_intersection(Stream<T>&& other, Compare&& compare = Compare());

    size_t count();

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

    bool occupied() const { return bool(source_); }

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

};

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<RepeatedStreamProvider, R>
        (std::forward<R>(value));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value, size_t times) {
    return MakeStream::repeat(value).limit(times);
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::cycle(Iterator begin, Iterator end) {
    using T = IteratorType<Iterator>;
    return MakeStream::repeat(make_tuple(begin, end))
        .flat_map(splat([](Iterator b, Iterator e) {
            return MakeStream::from(b, e);
        }));
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(const Container& cont) {
    return MakeStream::cycle(std::begin(cont), std::end(cont));
}


template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(Container&& cont) {
    return MakeStream::cycle(std::begin(cont), std::end(cont));
}

template<typename Generator>
Stream<ReturnType<Generator>> MakeStream::generate(Generator&& generator) {
    using T = ReturnType<Generator>;
    return make_stream_provider<GeneratedStreamProvider, T, Generator>
        (std::forward<Generator>(generator));
}

template<typename T, typename Function>
Stream<RemoveRef<T>> MakeStream::iterate(T&& initial, Function&& function) {
    using R = RemoveRef<T>;
    return make_stream_provider<IteratedStreamProvider, R, Function>
        (std::forward<T>(initial), std::forward<Function>(function));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start), [](R& value) {
            return ++value;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, T&& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [inc = std::forward<T>(increment)](R& value) {
            return value + inc;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, const T& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [&inc = increment](R& value) {
            return value + inc;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::singleton(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<SingletonStreamProvider, R>
        (std::forward<R>(value));
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::from(Iterator begin, Iterator end) {
    return {begin, end};
}

template<typename T>
Stream<T> MakeStream::from(T* array, std::size_t length) {
    return Stream<T>(array, array + length);
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::from(const Container& cont) {
    return {std::begin(cont), std::end(cont)};
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::from(Container&& cont) {
    return {std::begin(cont), std::end(cont)};
}

template<typename T>
Stream<T> MakeStream::from(std::initializer_list<T> init) {
    return Stream<T>(init.begin(), init.end());
}

template<typename T>
Stream<T>::Stream()
    : source_(make_stream_provider<EmptyStreamProvider, T>()) {}

template<typename T>
template<typename Iterator>
Stream<T>::Stream(Iterator begin, Iterator end)
    : source_(std::make_unique<IteratorStreamProvider<T, Iterator>>(
        begin, end)) {}


template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::filter(Predicate&& predicate) {
    check_vacant("filter");
    return make_stream_provider<FilteredStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate));
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::take_while(Predicate&& predicate) {
    check_vacant("take_while");
    return make_stream_provider<TakeWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate));
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::drop_while(Predicate&& predicate) {
    check_vacant("drop_while");
    return make_stream_provider<DropWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate));
}

template<typename T>
Stream<T> Stream<T>::slice(size_t start, size_t end, size_t increment) {
    check_vacant("slice");
    return make_stream_provider<SlicedStreamProvider, T>
        (std::move(source_), start, end, increment);
}

template<typename T>
template<typename Action>
Stream<T> Stream<T>::peek(Action&& action) {
    check_vacant("peek");
    return make_stream_provider<PeekStreamProvider, T, Action>
        (std::move(source_), std::forward<Action>(action));
}

template<typename T>
template<typename Transform>
Stream<ReturnType<Transform, T&>> Stream<T>::map(Transform&& transform) {
    using Result = ReturnType<Transform, T&>;
    static_assert(!std::is_void<Result>::value,
        "Return type of the mapping function cannot be void.");

    check_vacant("map");
    return make_stream_provider<MappedStreamProvider, Result, Transform, T>
        (std::move(source_), std::forward<Transform>(transform));
}

template<typename T>
template<typename Transform>
Stream<StreamType<ReturnType<Transform, T&>>>
Stream<T>::flat_map(Transform&& transform) {
    using Result = ReturnType<Transform, T&>;
    static_assert(IsStream<Result>::value,
        "Flat map must be passed a function which returns a stream");
    using S = StreamType<Result>;

    check_vacant("flat_map");
    return make_stream_provider<FlatMappedStreamProvider, S, Transform, T>
        (std::move(source_), std::forward<Transform>(transform));
}

template<typename T>
Stream<T> Stream<T>::limit(std::size_t length) {
    check_vacant("limit");
    return make_stream_provider<LimitedStreamProvider, T>
        (std::move(source_), length);
}

template<typename T>
Stream<T> Stream<T>::skip(std::size_t amount) {
    check_vacant("skip");
    return make_stream_provider<SkippedStreamProvider, T>
        (std::move(source_), amount);
}

template<typename T>
template<typename Equal>
Stream<T> Stream<T>::adjacent_distinct(Equal&& equal) {
    check_vacant("adjacent_distinct");
    return make_stream_provider<AdjacentDistinctStreamProvider, T, Equal>
        (std::move(source_), std::forward<Equal>(equal));
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::sort(Compare&& comparator) {
    check_vacant("sort");
    return make_stream_provider<SortedStreamProvider, T, Compare>
        (std::move(source_), std::forward<Compare>(comparator));
}

template<typename T>
Stream<T> Stream<T>::state_point() {
    check_vacant("state_point");
    return make_stream_provider<StatefulStreamProvider, T>
        (std::move(source_));
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::distinct(Compare&& comparator) {
    check_vacant("distinct");
    return make_stream_provider<DistinctStreamProvider, T, Compare>
        (std::move(source_), std::forward<Compare>(comparator));
}

template<typename T>
template<typename Iterator>
Stream<T> Stream<T>::concat(Iterator begin, Iterator end) {
    return concat(Stream<T>(begin, end));
}

template<typename T>
Stream<T> Stream<T>::concat(Stream<T>&& other) {
    check_vacant("concat");
    auto concat_ptr = dynamic_cast<ConcatenatedStreamProvider<T>*>(source_.get());
    if(concat_ptr) {
        concat_ptr->concat(std::move(other.source_));
        return std::move(*this);
    }
    return make_stream_provider<ConcatenatedStreamProvider, T>
        (std::move(source_), std::move(other.source_));
}

template<typename T>
Stream<T> Stream<T>::pad(T&& padding) {
    return concat(MakeStream::repeat(std::forward<T>(padding)));
}


template<typename T>
Stream<GroupResult<T, 2>> Stream<T>::pairwise() {
    check_vacant("pairwise");
    return grouped<2>();
}

template<typename T>
template<size_t N>
Stream<GroupResult<T, N>> Stream<T>::grouped() {
    using GroupType = GroupResult<T, N>;

    check_vacant("grouped");
    return std::move(StreamProviderPtr<GroupType>(
        new GroupedStreamProvider<T, N>(std::move(source_))));
}

template<typename T>
template<typename Subtractor>
Stream<ReturnType<Subtractor, T&, T&>>
Stream<T>::adjacent_difference(Subtractor&& subtract) {
    using Result = ReturnType<Subtractor, T&, T&>;
    static_assert(!std::is_void<Result>::value,
        "Return type of the subtraction cannot be void.");

    check_vacant("adjacent_difference");
    return std::move(StreamProviderPtr<Result>(
        new AdjacentDifferenceStreamProvider<T, Subtractor>(
            std::move(source_), std::forward<Subtractor>(subtract))));
}

template<typename T>
template<typename Adder>
Stream<T> Stream<T>::partial_sum(Adder&& add) {
    check_vacant("partial_sum");
    return make_stream_provider<PartialSumStreamProvider, T, Adder>
        (std::move(source_), std::forward<Adder>(add));
}

template<typename T>
template<typename Other, typename Function>
Stream<ReturnType<Function, T&, Other&>> Stream<T>::zip_with(
        Stream<Other>&& other, Function&& zipper) {
    using Result = ReturnType<Function, T&, Other&>;
    static_assert(!std::is_void<Result>::value,
        "Return type of the zipping function cannot be void.");

    check_vacant("zip_with");
    return std::move(StreamProviderPtr<Result>(
        new ZippedStreamProvider<T, Other, Function>(
            std::move(source_), std::move(other.source_),
            std::forward<Function>(zipper))));
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::merge_with(Stream<T>&& other, Compare&& compare) {
    check_vacant("merge_with");
    return make_stream_provider<MergedStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare), true);
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::set_union(Stream<T>&& other, Compare&& compare) {
    check_vacant("set_union");
    return make_stream_provider<MergedStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare), false);
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::set_intersection(Stream<T>&& other, Compare&& compare) {
    check_vacant("set_intersection");
    return make_stream_provider<SetIntersectionStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare));
}

template<typename T>
size_t Stream<T>::count() {
    check_vacant("count");
    size_t count = 0;
    while(source_->advance()) {
        source_->get();
        count++;
    }
    return count;
}

template<typename T>
template<typename Predicate>
bool Stream<T>::any(Predicate&& predicate) {
    check_vacant("any");
    while(source_->advance()) {
        if(predicate(*source_->get())) {
            return true;
        }
    }
    return false;
}

template<typename T>
template<typename Predicate>
bool Stream<T>::all(Predicate&& predicate) {
    check_vacant("all");
    while(source_->advance()) {
        if(!predicate(*source_->get())) {
            return false;
        }
    }
    return true;
}

template<typename T>
template<typename Predicate>
bool Stream<T>::none(Predicate&& predicate) {
    check_vacant("none");
    return !any(std::forward<Predicate>(predicate));
}

template<typename T>
template<typename OutputIterator>
void Stream<T>::copy_to(OutputIterator out) {
    check_vacant("copy_to");
    while(source_->advance()) {
        *out = *source_->get();
        out++;
    }
}

template<typename T>
template<typename OutputIterator>
void Stream<T>::move_to(OutputIterator out) {
    check_vacant("move_to");
    while(source_->advance()) {
        *out = std::move(*source_->get());
        out++;
    }
}

template<typename T>
void Stream<T>::print_to(std::ostream& os, const char* delimiter) {
    check_vacant("print_to");
    copy_to(std::ostream_iterator<T>(os, delimiter));
}


template<typename T>
std::vector<T> Stream<T>::to_vector() {
    check_vacant("to_vector");
    std::vector<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
std::list<T> Stream<T>::to_list() {
    check_vacant("to_list");
    std::list<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
template<typename Function>
void Stream<T>::for_each(Function&& function) {
    check_vacant("for_each");
    while(source_->advance()) {
        function(*source_->get());
    }
}

template<typename T>
void Stream<T>::check_vacant(const std::string& method) {
    if(!occupied())
        throw VacantStreamException(method);
}

#include "StreamAlgebra.h"

#endif
