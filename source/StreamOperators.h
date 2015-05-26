#ifndef SCHEINERMAN_STREAM_STREAM_OPERATORS_H
#define SCHEINERMAN_STREAM_STREAM_OPERATORS_H

#include <functional>

namespace stream {
namespace op {

#define SOURCE_TYPE(stream) \
    typename RemoveRef<decltype(stream)>::provider_t

#define CLASS_SPECIALIZATIONS(operation) \
    template<typename R, typename C> auto operation (R (C::*member)()) \
        { return operation (std::mem_fn(member)); } \
    template<typename R, typename C> auto operation (R (C::*member)() const) \
        { return operation (std::mem_fn(member)); }

template<typename Predicate>
auto filter(Predicate&& predicate) {
    return make_operator("stream::op::filter", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::filter<Source, Predicate>>
            (std::move(stream.source()), std::forward<Predicate>(predicate));
    });
}

auto filter() {
    return filter(to_bool);
}

CLASS_SPECIALIZATIONS(filter);

template<typename Predicate>
auto take_while(Predicate&& predicate) {
    return make_operator("stream::op::take_while", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::take_while<Source, Predicate>>
            (std::move(stream.source()), std::forward<Predicate>(predicate));
    });
}

auto take_while() {
    return take_while(to_bool);
}

CLASS_SPECIALIZATIONS(take_while);

template<typename Predicate>
auto drop_while(Predicate&& predicate) {
    return make_operator("stream::op::drop_while", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::drop_while<Source, Predicate>>
            (std::move(stream.source()), std::forward<Predicate>(predicate));
    });
}

auto drop_while() {
    return drop_while(to_bool);
}

CLASS_SPECIALIZATIONS(drop_while);

auto slice(std::size_t start, std::size_t end, std::size_t increment = 1) {
    return make_operator("stream::op::slice", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::slice<Source>>
            (std::move(stream.source()), start, end, increment, false);
    });
}

auto slice_to_end(std::size_t start, std::size_t increment) {
    return make_operator("stream::op::slice", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::slice<Source>>
            (std::move(stream.source()), start, 0, increment, true);
    });
}

auto limit(std::size_t length) {
    return slice(0, length).rename("stream::op::limit");
}

auto skip(std::size_t amount) {
    return slice_to_end(amount, 1).rename("stream::op::skip");
}

template<typename Function>
auto map_(Function&& function) {
    return make_operator("stream::op::map_", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::map<Source, Function>>
            (std::move(stream.source()), std::forward<Function>(function));
    });
}

CLASS_SPECIALIZATIONS(map_);

template<typename Action>
auto peek(Action&& action) {
    return make_operator("stream::op::peek", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::peek<Source, Action>>
            (std::move(stream.source()), std::forward<Action>(action));
    });
}

CLASS_SPECIALIZATIONS(peek);

template<typename Transform>
auto flat_map(Transform&& transform) {
    return make_operator("stream::op::flat_map", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::flat_map<Source, Transform>>
            (std::move(stream.source()), std::forward<Transform>(transform));
    });
}

CLASS_SPECIALIZATIONS(flat_map);

template<typename Equal = std::equal_to<void>>
auto adjacent_distinct(Equal&& equal = Equal()) {
    return make_operator("stream::op::adjacent_distinct", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::adjacent_distinct<Source, Equal>>
            (std::move(stream.source()), std::forward<Equal>(equal));
    });
}

template<typename Subtractor = std::minus<void>>
auto adjacent_difference(Subtractor&& subtract = Subtractor()) {
    return make_operator("stream::op::adjacent_difference", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::adjacent_difference<Source, Subtractor>>
            (std::move(stream.source()), std::forward<Subtractor>(subtract));
    });
}

template<typename Adder = std::plus<void>>
auto partial_sum(Adder&& add = Adder()) {
    return make_operator("stream::op::partial_sum", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::partial_sum<Source, Adder>>
            (std::move(stream.source()), std::forward<Adder>(add));
    });
}

template<typename TailSource>
auto concat(Stream<TailSource>&& tail) {
    return make_operator("stream::op::concat", [tail = std::move(tail)] (auto&& head) mutable {
        if(!tail.occupied())
            throw VacantStreamException("stream::op::concat");

        using HeadSource = SOURCE_TYPE(head);
        return provider::make_stream<provider::concatenate<HeadSource, TailSource>>
            (std::move(head.source()), std::move(tail.source()));
    });
}

template<size_t N>
auto group() {
    return make_operator("stream::op::group", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::group<Source, N>>
            (std::move(stream.source()));
    });
}

auto pairwise() {
    return group<2>().rename("stream::op::pairwise");
}

auto group(size_t N) {
    return make_operator("stream::op::group", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::dynamic_group<Source>>
            (std::move(stream.source()), N);
    });
}

template<size_t N>
auto overlap() {
    return make_operator("stream::op::overlap", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::overlap<Source, N>>
            (std::move(stream.source()));
    });
}

auto overlap(size_t N) {
    return make_operator("stream::op::overlap", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::dynamic_overlap<Source>>
            (std::move(stream.source()), N);
    });
}

namespace detail {

template<template<typename...> class Provider, typename RightSource, typename Join>
auto make_combination_operator(const std::string& name, Stream<RightSource>&& right, Join&& join) {
    return make_operator(name, [name, right = std::move(right), join] (auto&& left) mutable {
        if (!right.occupied())
            throw VacantStreamException(name);

        using LeftSource = SOURCE_TYPE(left);
        return provider::make_stream<Provider<LeftSource, RightSource, Join>>
            (std::move(left.source()), std::move(right.source()), std::forward<Join>(join));
    });
}

} /* namespace detail */

template<typename RightSource, typename Function = provider::detail::Zipper>
auto zip_with(Stream<RightSource>&& right, Function&& zipper = Function()) {
    return detail::make_combination_operator<provider::zip>
        ("stream::op::zip_with", std::move(right), std::forward<Function>(zipper));
}

template<typename RightSource, typename Less = std::less<typename RightSource::element>>
auto merge_with(Stream<RightSource>&& right, Less&& less = Less()) {
    return detail::make_combination_operator<provider::merge>(
        "stream::op::merge_with", std::move(right), std::forward<Less>(less));
}

template<typename RightSource, typename Less = std::less<typename RightSource::element>>
auto union_with(Stream<RightSource>&& right, Less&& less = Less()) {
    return detail::make_combination_operator<provider::union_>
        ("stream::op::union_with", std::move(right), std::forward<Less>(less));
}

template<typename RightSource, typename Less = std::less<typename RightSource::element>>
auto intersect_with(Stream<RightSource>&& right, Less&& less = Less()) {
    return detail::make_combination_operator<provider::intersection>(
        "stream::op::intersect_with", std::move(right), std::forward<Less>(less));
}

template<typename RightSource, typename Less = std::less<typename RightSource::element>>
auto difference_with(Stream<RightSource>&& right, Less&& less = Less()) {
    return detail::make_combination_operator<provider::difference>
        ("stream::op::union_with", std::move(right), std::forward<Less>(less));
}

template<typename RightSource, typename Less = std::less<typename RightSource::element>>
auto symmetric_difference_with(Stream<RightSource>&& right, Less&& less = Less()) {
    return detail::make_combination_operator<provider::symmetric_difference>(
        "stream::op::symmetric_difference_with", std::move(right), std::forward<Less>(less));
}

auto state_point() {
    return make_operator("stream::op::state_point", [=](auto&& stream) {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::stateful<Source>>
            (std::move(stream.source()));
    });
}

template<typename Less = std::less<void>>
auto sort(Less&& less = Less()) {
    return make_operator("stream::op::sort", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::sort<Source, Less>>
            (std::move(stream.source()), std::forward<Less>(less));
    });
}

template<typename Less = std::less<void>>
auto distinct(Less&& less = Less()) {
    return make_operator("stream::op::distinct", [=](auto&& stream) mutable {
        using Source = SOURCE_TYPE(stream);
        return provider::make_stream<provider::distinct<Source, Less>>
            (std::move(stream.source()), std::forward<Less>(less));
    });
}

#undef SOURCE_TYPE
#undef CLASS_SPECIALIZATIONS

} /* namespace op */
} /* namespace stream */

#endif
