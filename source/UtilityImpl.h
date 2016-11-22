#ifndef SCHEINERMAN_STREAM_UTILITY_IMPL_H
#define SCHEINERMAN_STREAM_UTILITY_IMPL_H

#include <iostream>
#include <tuple>

namespace stream {

namespace detail {

template<typename T>
struct SeqTail {};

template<typename T, T Head, T... Tail>
struct SeqTail<std::integer_sequence<T, Head, Tail...>> {
    using type = std::integer_sequence<T, Tail...>;
};

} /* namespace detail */

// ===================== N-Tuple ====================

namespace detail {

template<typename A, typename B>
struct Append {
};

template<template<typename...> class Base, typename... Args, typename Head>
struct Append<Head, Base<Args...>> {
    using Type = Base<Head, Args...>;
};

template<typename T, size_t N>
struct NTupleImpl {
    using SubType = typename NTupleImpl<T, N-1>::Type;
    using Type = typename Append<T, SubType>::Type;
};

template<typename T>
struct NTupleImpl<T, 1> {
    using Type = std::tuple<T>;
};

} /* namespace detail*/

// ================= Tuple Printing =================

template<size_t index, size_t last, typename... Args>
struct PrintTuple {
    static void print(std::ostream& os, const std::tuple<Args...>& tuple) {
        os << std::get<index>(tuple) << ", ";
        PrintTuple<index + 1, last, Args...>::print(os, tuple);
    }
};

template<size_t last, typename... Args>
struct PrintTuple<0, last, Args...> {
    static void print(std::ostream& os, const std::tuple<Args...>& tuple) {
        os << "(" << std::get<0>(tuple) << ", ";
        PrintTuple<1, last, Args...>::print(os, tuple);
    }
};

template<size_t index, typename... Args>
struct PrintTuple<index, index, Args...> {
    static void print(std::ostream& os, const std::tuple<Args...>& tuple) {
        os << std::get<index>(tuple) << ")";
    }
};

// ================= Tuple Splatting =================

template<typename... Types>
struct TupleWrapper {
    using Type = std::tuple<Types...>;
    enum { Length = sizeof...(Types) };
};

template<typename Return, typename Function, size_t index, size_t last,
         typename Tuple, typename... Args>
struct SplatTuple {
    static_assert(index >= 0,
        "Splat tuple index cannot be less than 0.");
    static_assert(index < Tuple::Length,
        "Splat tuple index can not be greater than the tuple elements.");

    using TupleType = typename Tuple::Type;

    static Return splat(Function&& function, const TupleType& tuple,
                        Args&&... args) {
        return SplatTuple<Return, Function, index + 1, last, Tuple,
                          Args..., decltype(std::get<index>(tuple))>
            ::splat(std::forward<Function>(function), tuple,
                    std::forward<Args>(args)..., std::get<index>(tuple));
    }
};

template<typename Return, typename Function, size_t last,
         typename Tuple>
struct SplatTuple<Return, Function, 0, last, Tuple> {
    using TupleType = typename Tuple::Type;

    static Return splat(Function&& function, const TupleType& tuple) {
        return SplatTuple<Return, Function, 1, last, Tuple,
                          decltype(std::get<0>(tuple))>
            ::splat(std::forward<Function>(function), tuple, std::get<0>(tuple));
    }
};

template<typename Return, typename Function, size_t last,
         typename Tuple, typename... Args>
struct SplatTuple<Return, Function, last, last, Tuple, Args...> {
    using TupleType = typename Tuple::Type;

    static Return splat(Function&& function, const TupleType& tuple,
                        Args&&... args) {
        return function(std::forward<Args>(args)...);
    }
};

template<typename Function, typename... Types>
inline auto apply_tuple(Function&& function, const std::tuple<Types...>& tuple)
        -> decltype(function(std::declval<Types>()...)) {

    using Return = decltype(function(std::declval<Types>()...));
    return SplatTuple<Return, Function, 0, sizeof...(Types),
                      TupleWrapper<Types...>>
        ::splat(std::forward<Function>(function), tuple);

}

template<typename Function, typename L, typename R>
inline auto apply_tuple(Function&& function, const std::pair<L, R>& pair) {
    return function(pair.first, pair.second);
}

template<typename Function>
struct SplattedFunction {

public:
    SplattedFunction(Function&& function) : function_(function) {}

    template<typename... Args>
    auto operator() (const std::tuple<Args...>& tuple)
            -> decltype(std::declval<Function>()(std::declval<Args>()...)) {

        return apply_tuple(function_, tuple);
    }

    template<typename L, typename R>
    auto operator() (const std::pair<L, R>& pair)
            -> decltype(std::declval<Function>()(pair.first, pair.second)) {

        return apply_tuple(function_, pair);
    }

private:
    Function function_;

};

// More generic splatting

template<typename... Types>
struct SplattableTuple {
    SplattableTuple(const std::tuple<Types...>& value_) : value(value_) {}

    const std::tuple<Types...>& value;
};

template<size_t index, size_t last, typename First, typename... Args>
struct ArgsToTuple {
    static auto tuplize(First&& first_arg, Args&&... args) {
        return std::tuple_cat(std::make_tuple(std::forward<First>(first_arg)),
            ArgsToTuple<index + 1, last, Args...>
                ::tuplize(std::forward<Args>(args)...));
    }
};

template<size_t last, typename Last>
struct ArgsToTuple<last, last, Last> {
    static auto tuplize(Last&& last_arg) {
        return std::make_tuple(std::forward<Last>(last_arg));
    }
};

template<size_t index, size_t last, typename... Types, typename... Args>
struct ArgsToTuple<index, last, SplattableTuple<Types...>, Args...> {
    using InTuple = SplattableTuple<Types...>;
    static auto tuplize(InTuple&& first, Args&&... args) {
        return std::tuple_cat(first.value,
            ArgsToTuple<index + 1, last, Args...>
                ::tuplize(std::forward<Args>(args)...));
    }
};

template<size_t last, typename... Types>
struct ArgsToTuple<last, last, SplattableTuple<Types...>> {
    static std::tuple<Types...> tuplize(SplattableTuple<Types...>&& first) {
        return first.value;
    }
};

template<typename... Args>
inline auto args2tuple(Args&&... args) {
    return ArgsToTuple<0, sizeof...(Args)-1, Args...>
        ::tuplize(std::forward<Args>(args)...);
}

template<typename Function>
struct SplattableFunction {

public:
    SplattableFunction(Function&& function) : function_(function) {}

    template<typename... Args>
    auto operator() (Args&&... args) {
        return apply_tuple(function_,
            ArgsToTuple<0, sizeof...(Args)-1, Args...>
                ::tuplize(std::forward<Args>(args)...));
    }

private:
    Function function_;

};

} /* namespace stream */

#endif
