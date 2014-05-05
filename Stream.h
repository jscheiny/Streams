#ifndef STREAM_H
#define STREAM_H

#include "Providers.h"
#include "Utility.h"

#include <functional>
#include <type_traits>
#include <iostream>
#include <iterator>
#include <vector>
#include <list>

template<typename T>
class Stream {

public:
    using ElementType = T;

    static Stream<T> repeat(T&& value);

    template<typename Generator>
    static Stream<T> generate(Generator&& generator);

    template<typename Function>
    static Stream<T> iterate(T initial, Function&& function);

    Stream();

    template<typename Iterator>
    Stream(Iterator begin, Iterator end);

    template<typename Predicate>
    Stream<T> filter(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> take_while(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> take_until(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> drop_while(Predicate&& predicate);

    template<typename Predicate>
    Stream<T> drop_until(Predicate&& predicate);

    template<typename Action>
    Stream<T> peek(Action&& action);

    template<typename Transform>
    Stream<ReturnType<Transform, T&>> map(Transform&& transform);

    template<typename Transform>
    Stream<StreamType<ReturnType<Transform, T&>>>
    flat_map(Transform&& transform);

    Stream<T> limit(std::size_t length);

    Stream<T> skip(std::size_t amount);

    template<typename Compare = std::less<T>>
    Stream<T> sort(Compare&& comparator = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> distinct(Compare&& comparator = Compare());

    // Is there a better name for this? force_state()?
    Stream<T> state_point();

    template<typename Subtractor = Minus<T>> // std::minus<void> in c++14
    Stream<ReturnType<Subtractor, T&, T&>>
    adjacent_difference(Subtractor&& subtract = Subtractor());

    template<typename Adder = std::plus<T>>
    Stream<T> partial_sum(Adder&& add = Adder());

    template<typename Iterator>
    Stream<T> concat(Iterator begin, Iterator end);

    Stream<T> concat(Stream<T>&& other);

    template<typename Other>
    Stream<ZipResult<T, Other>> zip_with(Stream<Other>&& other);

    template<typename Compare = std::less<T>>
    Stream<T> merge_with(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> set_union(Stream<T>&& other, Compare&& compare = Compare());

    template<typename Compare = std::less<T>>
    Stream<T> set_intersection(Stream<T>&& other, Compare&& compare = Compare());

    size_t count();

    template<typename OutputIterator>
    void copy_to(OutputIterator out);

    template<typename OutputIterator>
    void move_to(OutputIterator out);

    void print_to(std::ostream& os, const char* delimiter = " ");

    std::vector<T> as_vector();

    std::list<T> as_list();

    template<typename Function>
    void for_each(Function&& function);

    bool occupied() const {
        return source_;
    }

    template<typename>
    friend class Stream;

    template<typename, typename, typename>
    friend class FlatMappedStreamProvider;

private:
    Stream(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

    StreamProviderPtr<T> source_;

};

template<typename T>
Stream<T>::Stream()
    : source_(make_stream_provider<EmptyStreamProvider, T>()) {}

template<typename T>
template<typename Iterator>
Stream<T>::Stream(Iterator begin, Iterator end)
    : source_(std::make_unique<IteratorStreamProvider<T, Iterator>>(
        begin, end)) {}

template<typename T>
Stream<T> Stream<T>::repeat(T&& value) {
    return make_stream_provider <RepeatedStreamProvider, T>
        (std::forward<T>(value));
}

template<typename T>
template<typename Generator>
Stream<T> Stream<T>::generate(Generator&& generator) {
    return make_stream_provider <GeneratedStreamProvider, T, Generator>
        (std::forward<Generator>(generator));
}

template<typename T>
template<typename Function>
Stream<T> Stream<T>::iterate(T initial, Function&& function) {
    return make_stream_provider <IteratedStreamProvider, T, Function>
        (std::forward<T>(initial), std::forward<Function>(function));
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::filter(Predicate&& predicate) {
    return make_stream_provider <FilteredStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate));
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::take_while(Predicate&& predicate) {
    return make_stream_provider <TakeWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate), false);
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::take_until(Predicate&& predicate) {
    return make_stream_provider <TakeWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate), true);
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::drop_while(Predicate&& predicate) {
    return make_stream_provider <DropWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate), false);
}

template<typename T>
template<typename Predicate>
Stream<T> Stream<T>::drop_until(Predicate&& predicate) {
    return make_stream_provider <DropWhileStreamProvider, T, Predicate>
        (std::move(source_), std::forward<Predicate>(predicate), true);
}

template<typename T>
template<typename Action>
Stream<T> Stream<T>::peek(Action&& action) {
    return make_stream_provider <PeekStreamProvider, T, Action>
        (std::move(source_), std::forward<Action>(action));
}

template<typename T>
template<typename Transform>
Stream<ReturnType<Transform, T&>> Stream<T>::map(Transform&& transform) {
    using Result = ReturnType<Transform, T>;
    return make_stream_provider <MappedStreamProvider, Result, Transform, T>
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
    return make_stream_provider <FlatMappedStreamProvider, S, Transform, T>
        (std::move(source_), std::forward<Transform>(transform));
}

template<typename T>
Stream<T> Stream<T>::limit(std::size_t length) {
    return make_stream_provider <LimitedStreamProvider, T>
        (std::move(source_), length);
}

template<typename T>
Stream<T> Stream<T>::skip(std::size_t amount) {
    return make_stream_provider <SkippedStreamProvider, T>
        (std::move(source_), amount);
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::sort(Compare&& comparator) {
    return make_stream_provider <SortedStreamProvider, T, Compare>
        (std::move(source_), std::forward<Compare>(comparator));
}

template<typename T>
Stream<T> Stream<T>::state_point() {
    return make_stream_provider <StatefulStreamProvider, T>
        (std::move(source_));
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::distinct(Compare&& comparator) {
    return make_stream_provider <DistinctStreamProvider, T, Compare>
        (std::move(source_), std::forward<Compare>(comparator));
}

template<typename T>
template<typename Iterator>
Stream<T> Stream<T>::concat(Iterator begin, Iterator end) {
    return concat(Stream<T>(begin, end));
}

template<typename T>
Stream<T> Stream<T>::concat(Stream<T>&& other) {
    return make_stream_provider <ConcatenatedStreamProvider, T>
        (std::move(source_), std::move(other.source_));
}

template<typename T>
template<typename Subtractor>
Stream<ReturnType<Subtractor, T&, T&>>
Stream<T>::adjacent_difference(Subtractor&& subtract) {
    using Result = ReturnType<Subtractor, T&, T&>;
    return std::move(StreamProviderPtr<Result>(
        new AdjacentDifferenceStreamProvider<T, Subtractor>(
            std::move(source_), std::forward<Subtractor>(subtract))));
}

template<typename T>
template<typename Adder>
Stream<T> Stream<T>::partial_sum(Adder&& add) {
    return make_stream_provider<PartialSumStreamProvider, T, Adder>
        (std::move(source_), std::forward<Adder>(add));
}

template<typename T>
template<typename Other>
Stream<ZipResult<T, Other>> Stream<T>::zip_with(Stream<Other>&& other) {
    using Result = ZipResult<T, Other>;
    return std::move(StreamProviderPtr<Result>(
        new ZippedStreamProvider<T, Other>(
            std::move(source_), std::move(other.source_))));
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::merge_with(Stream<T>&& other, Compare&& compare) {
    return make_stream_provider <MergedStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare), true);
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::set_union(Stream<T>&& other, Compare&& compare) {
    return make_stream_provider <MergedStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare), false);
}

template<typename T>
template<typename Compare>
Stream<T> Stream<T>::set_intersection(Stream<T>&& other, Compare&& compare) {
    return make_stream_provider <SetIntersectionStreamProvider, T, Compare>
        (std::move(source_), std::move(other.source_),
         std::forward<Compare>(compare));
}

template<typename T>
template<typename OutputIterator>
void Stream<T>::copy_to(OutputIterator out) {
    while(source_->advance()) {
        *out = *source_->get();
        out++;
    }
}

template<typename T>
template<typename OutputIterator>
void Stream<T>::move_to(OutputIterator out) {
    while(source_->advance()) {
        *out = std::move(*source_->get());
        out++;
    }
}

template<typename T>
void Stream<T>::print_to(std::ostream& os, const char* delimiter) {
    copy_to(std::ostream_iterator<T>(os, delimiter));
}


template<typename T>
std::vector<T> Stream<T>::as_vector() {
    std::vector<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
std::list<T> Stream<T>::as_list() {
    std::list<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
template<typename Function>
void Stream<T>::for_each(Function&& function) {
    while(source_->advance()) {
        function(*source_->get());
    }
}

template<typename T>
size_t Stream<T>::count() {
    size_t count = 0;
    while(source_->advance()) {
        source_->get();
        count++;
    }
    return count;
}


#endif
