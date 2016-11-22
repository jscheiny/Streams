#ifndef SCHEINERMAN_STREAM_STREAM_OPERATORS_H
#define SCHEINERMAN_STREAM_STREAM_OPERATORS_H

#include <functional>

namespace stream {
namespace op {

#define CLASS_SPECIALIZATIONS(operation) \
    template<typename R, typename C> auto operation (R (C::*member)()) \
        { return operation (std::mem_fn(member)); } \
    template<typename R, typename C> auto operation (R (C::*member)() const) \
        { return operation (std::mem_fn(member)); }

template<typename Predicate>
inline auto filter(Predicate&& predicate) {
    return make_operator("stream::op::filter", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Filter, T, Predicate>(
                std::move(stream.getSource()), std::forward<Predicate>(predicate))));
    });
}

inline auto filter() {
    return filter([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(filter);

template<typename Predicate>
inline auto take_while(Predicate&& predicate) {
    return make_operator("stream::op::take_while", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::TakeWhile, T, Predicate>
                (std::move(stream.getSource()), std::forward<Predicate>(predicate))));
    });
}

inline auto take_while() {
    return take_while([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(take_while);

template<typename Predicate>
inline auto drop_while(Predicate&& predicate) {
    return make_operator("stream::op::drop_while", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::DropWhile, T, Predicate>(
                std::move(stream.getSource()), std::forward<Predicate>(predicate))));
    });
}

inline auto drop_while() {
    return drop_while([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(drop_while);

inline auto slice(std::size_t start, std::size_t end, std::size_t increment = 1) {
    return make_operator("stream::op::slice", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Slice, T>(
                std::move(stream.getSource()), start, end, increment, false)));
    });
}

inline auto slice_to_end(std::size_t start, std::size_t increment) {
    return make_operator("stream::op::slice", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Slice, T>(
                std::move(stream.getSource()), start, 0, increment, true)));
    });
}

inline auto limit(std::size_t length) {
    return slice(0, length).rename("stream::op::limit");
}

inline auto skip(std::size_t amount) {
    return slice_to_end(amount, 1).rename("stream::op::skip");
}

template<typename Function>
inline auto map_(Function&& function) {
    return make_operator("stream::op::map_", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        using Result = std::result_of_t<Function(T&&)>;
        static_assert(!std::is_void<Result>::value,
            "Return type of the mapping function cannot be void.");

        return Stream<Result>(std::move(
            make_stream_provider<provider::Map, Result, Function, T>(
                std::move(stream.getSource()), std::forward<Function>(function))));
    });
}

CLASS_SPECIALIZATIONS(map_);

template<typename Action>
inline auto peek(Action&& action) {
    return make_operator("stream::op::peek", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Peek, T, Action>(
                std::move(stream.getSource()), std::forward<Action>(action))));
    });
}

CLASS_SPECIALIZATIONS(peek);

template<typename Transform>
inline auto flat_map(Transform&& transform) {
    return make_operator("stream::op::flat_map", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        using Result = std::result_of_t<Transform(T&&)>;
        using S = StreamType<Result>;
        static_assert(!std::is_void<S>::value,
            "Flat map must be passed a function which returns a stream.");

        return Stream<S>(std::move(
            make_stream_provider<provider::FlatMap, S, Transform, T>(
                std::move(stream.getSource()), std::forward<Transform>(transform))));
    });
}

CLASS_SPECIALIZATIONS(flat_map);

template<typename Equal = std::equal_to<void>>
inline auto adjacent_distinct(Equal&& equal = Equal()) {
    return make_operator("stream::op::adjacent_distinct", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::AdjacentDistinct, T, Equal>(
                std::move(stream.getSource()), std::forward<Equal>(equal))));
    });
}

template<typename Subtractor = std::minus<void>>
inline auto adjacent_difference(Subtractor&& subtract = Subtractor()) {
    return make_operator("stream::op::adjacent_difference", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        using Result = std::result_of_t<Subtractor(T&, T&)>;
        return Stream<T>(std::move(StreamProviderPtr<Result>(
            new provider::AdjacentDifference<T, Subtractor>(
                std::move(stream.getSource()), std::forward<Subtractor>(subtract)))));
    });
}

template<typename Adder = std::plus<void>>
inline auto partial_sum(Adder&& add = Adder()) {
    return make_operator("stream::op::partial_sum", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::PartialSum, T, Adder>(
                std::move(stream.getSource()), std::forward<Adder>(add))));
    });
}

template<typename U>
inline auto concat(Stream<U>&& tail) {
    return make_operator("stream::op::concat", [tail = std::move(tail)] (auto&& head) mutable {
        if(!tail.occupied())
            throw VacantStreamException("stream::op::concat");
        using T = StreamType<decltype(head)>;
        static_assert(std::is_same<T, U>::value,
            "Cannot concatenate streams with different types.");

        auto concat_ptr = dynamic_cast<provider::Concatenate<T>*>(head.getSource().get());
        if(concat_ptr) {
            concat_ptr->concat(std::move(tail.getSource()));
            return std::move(head);
        }
        return Stream<T>(std::move(
            make_stream_provider<provider::Concatenate, T>(
                std::move(head.getSource()), std::move(tail.getSource()))));
    });
}

template<typename Iterator>
inline auto concat(Iterator begin, Iterator end) {
    return make_operator("stream::op::concat", [=](auto&& stream) {
        using I = IteratorType<Iterator>;
        using T = StreamType<decltype(stream)>;
        static_assert(std::is_same<T, I>::value,
            "Cannot concatenate streams with different types.");

        return stream | op::concat(Stream<T>(begin, end));
    });
}

template<size_t N>
inline auto group() {
    return make_operator("stream::op::group", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        using G = provider::detail::GroupResult<T, N>;
        return Stream<G>(std::move(StreamProviderPtr<G>(
            new provider::Group<T, N>(std::move(stream.getSource())))));
    });
}

inline auto pairwise() {
    return group<2>().rename("stream::op::pairwise");
}

inline auto group(size_t N) {
    return make_operator("stream::op::group", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        using G = std::vector<T>;
        return Stream<G>(std::move(StreamProviderPtr<G>(
            new provider::DynamicGroup<T>(std::move(stream.getSource()), N))));
    });
}

template<size_t N>
inline auto overlap() {
    return make_operator("stream::op::overlap", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        using O = NTuple<T, N>;
        return Stream<O>(std::move(StreamProviderPtr<O>(
            new provider::Overlap<T, N>(std::move(stream.getSource())))));
    });
}

inline auto overlap(size_t N) {
    return make_operator("stream::op::overlap", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        using O = std::deque<T>;
        return Stream<O>(std::move(StreamProviderPtr<O>(
            new provider::DynamicOverlap<T>(std::move(stream.getSource()), N))));
    });
}

template<typename R, typename Function = provider::detail::Zipper>
inline auto zip_with(Stream<R>&& right, Function&& zipper = Function()) {
    return make_operator("stream::op::zip_with", [right = std::move(right), zipper]
    (auto&& left) mutable {
        if(!right.occupied())
            throw VacantStreamException("stream::op::zip_with");

        using L = StreamType<decltype(left)>;
        using Result = std::result_of_t<Function(L&&, R&&)>;
        static_assert(!std::is_void<Result>::value,
            "Return type of the zipping function cannot be void.");

        return Stream<Result>(std::move(StreamProviderPtr<Result>(
            new provider::Zip<L, R, Function>(
                std::move(left.getSource()), std::move(right.getSource()),
                std::forward<Function>(zipper)))));
    });
}

namespace detail {

template<template<typename...> class Provider, typename T, typename Less>
inline auto make_set_operator(const std::string& name, Stream<T>&& right, Less&& less) {
    return make_operator(name, [name, right = std::move(right), less] (auto&& left) mutable {
        if(!right.occupied())
            throw VacantStreamException(name);
        using S = StreamType<decltype(left)>;
        static_assert(std::is_same<T, S>::value,
            "Cannot compute combination of streams of different types.");

        return Stream<T>(std::move(
            make_stream_provider<Provider, T, Less>(
                std::move(left.getSource()), std::move(right.getSource()),
                std::forward<Less>(less))));
    });
}

} /* namespace detail */

template<typename T, typename Less = std::less<T>>
inline auto merge_with(Stream<T>&& right, Less&& less = Less()) {
    return detail::make_set_operator<provider::Merge>(
        "stream::op::merge_with", std::move(right), std::forward<Less>(less));
}

template<typename T, typename Less = std::less<T>>
inline auto union_with(Stream<T>&& right, Less&& less = Less()) {
    return detail::make_set_operator<provider::Union>(
        "stream::op::union_with", std::move(right), std::forward<Less>(less));
}

template<typename T, typename Less = std::less<T>>
inline auto intersect_with(Stream<T>&& right, Less&& less = Less()) {
    return detail::make_set_operator<provider::Intersection>(
        "stream::op::intersect_with", std::move(right), std::forward<Less>(less));
}

template<typename T, typename Less = std::less<T>>
inline auto difference_with(Stream<T>&& right, Less&& less = Less()) {
    return detail::make_set_operator<provider::Difference>(
        "stream::op::difference_with", std::move(right), std::forward<Less>(less));
}

template<typename T, typename Less = std::less<T>>
inline auto symmetric_difference_with(Stream<T>&& right, Less&& less = Less()) {
    return detail::make_set_operator<provider::SymmetricDifference>(
        "stream::op::symmetric_difference_with", std::move(right), std::forward<Less>(less));
}

inline auto state_point() {
    return make_operator("stream::op::state_point", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(make_stream_provider<provider::Stateful, T>(
            std::move(stream.getSource()))));
    });
}

template<typename Less = std::less<void>>
inline auto sort(Less&& less = Less()) {
    return make_operator("stream::op::sort", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Sort, T, Less>(
                std::move(stream.getSource()), std::forward<Less>(less))));
    });
}

template<typename Less = std::less<void>>
inline auto distinct(Less&& less = Less()) {
    return make_operator("stream::op::distinct", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        return Stream<T>(std::move(
            make_stream_provider<provider::Distinct, T, Less>(
                std::move(stream.getSource()), std::forward<Less>(less))));
    });
}

#undef CLASS_SPECIALIZATIONS

} /* namespace op */
} /* namespace stream */

#endif
