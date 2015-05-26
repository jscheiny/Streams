#ifndef SCHEINERMAN_STREAM_UTILITY_H
#define SCHEINERMAN_STREAM_UTILITY_H

#include "StreamForward.h"
#include "UtilityImpl.h"

#include <iostream>
#include <iterator>
#include <tuple>

namespace stream {

bool to_bool(bool x) {
    return x;
}

template<typename Function>
struct InvertedPredicate {
    InvertedPredicate(Function&& fn) : function(fn) {}

    template<typename... Args>
    bool operator() (Args&&... args) {
        return !function(std::forward<Args>(args)...);
    }

    Function function;
};

template<typename Head, typename... Tail>
struct HeadImpl {
    using type = Head;
};

template<typename... Ts>
using Head = typename HeadImpl<Ts...>::type;

template<typename T, T N>
using make_1based_sequence = typename detail::SeqTail<std::make_integer_sequence<T, N>>::type;

template<typename Function>
InvertedPredicate<Function> not_(Function&& fn) {
    return {std::forward<Function>(fn)};
}

template<typename T, size_t N>
using NTuple = typename detail::NTupleImpl<T, N>::Type;

template<typename T>
using RemoveRef = std::remove_const_t<std::remove_reference_t<T>>;

template<typename Itr>
using IteratorType = typename std::iterator_traits<Itr>::value_type;

template<typename Container>
using ContainerType = IteratorType
    <decltype(std::begin(std::declval<Container>()))>;

template<typename T, typename Compare, bool Reverse = false>
class ComparePtrWrapper {
public:
    ComparePtrWrapper(Compare&& comparator) : comparator_(comparator) {}

    bool operator() (const std::shared_ptr<T>& left,
                     const std::shared_ptr<T>& right) {
        return comparator_(*left, *right);
    }

private:
    Compare comparator_;
};

template<typename T, typename Compare>
class ComparePtrWrapper<T, Compare, true> {
public:
    ComparePtrWrapper(Compare&& comparator) : comparator_(comparator) {}

    bool operator() (const std::shared_ptr<T>& left,
                     const std::shared_ptr<T>& right) {
        return comparator_(*right, *left);
    }

private:
    Compare comparator_;
};

template<typename... Args>
std::ostream& operator<< (std::ostream& os, const std::tuple<Args...>& tuple) {

    PrintTuple<0, std::tuple_size<std::tuple<Args...>>::value - 1, Args...>
        ::print(os, tuple);
    return os;

}

template<typename Function, typename... Types>
auto apply_tuple(Function&& function, const std::tuple<Types...>& tuple)
        -> decltype(function(std::declval<Types>()...));

template<typename Function, typename L, typename R>
auto apply_tuple(Function&& function, const std::pair<L, R>& pair)
        -> decltype(function(pair.first, pair.second));

template<typename Function>
SplattedFunction<Function> splat(Function&& function) {
    return SplattedFunction<Function>(std::forward<Function>(function));
}

template<typename... Types>
SplattableTuple<Types...> operator+(const std::tuple<Types...>& tup) {
    return SplattableTuple<Types...>(tup);
}

template<typename Function>
SplattableFunction<Function> splattable(Function&& function) {
    return SplattableFunction<Function>(std::forward<Function>(function));
}

template<typename T>
struct noop {
    T operator() () { return {}; }
};

template<>
struct noop<void> {
    void operator() () {}
};

// Adapted from:
// http://stackoverflow.com/questions/12015195/how-to-call-member-function-only-if-object-happens-to-have-it
#define GENERATE_METHOD_CHECK(name, qual)                                      \
namespace detail {                                                             \
                                                                               \
    template<typename T, typename ReturnType, typename... Args>                \
    class has_##name {                                                         \
        template<typename U, U> class check {};                                \
                                                                               \
        template<typename C>                                                   \
        static std::true_type                                                  \
        test(check<ReturnType (C::*)(Args...) qual, &C :: name >*);            \
                                                                               \
        template<typename C>                                                   \
        static std::false_type test(...);                                      \
                                                                               \
        using type = decltype(test<T>(0));                                     \
                                                                               \
        template<typename Default>                                             \
        static ReturnType                                                      \
        eval_impl(std::true_type, qual T& t, Args&&... args, Default&&) {      \
            return t.name(std::forward<Args>(args)...);                        \
        }                                                                      \
                                                                               \
        template<typename Default>                                             \
        static ReturnType                                                      \
        eval_impl(std::false_type, qual T& t, Args&&... args, Default&& def) { \
            return def();                                                      \
        }                                                                      \
                                                                               \
    public:                                                                    \
        constexpr static bool value = type::value;                             \
        template<typename Default = noop<ReturnType>>                          \
        static ReturnType                                                      \
        eval(qual T& t, Args&&... args, Default&& def = Default{}) {           \
            return eval_impl(type{}, t, std::forward<Args>(args)...,           \
                             std::forward<Default>(def));                      \
        }                                                                      \
    };                                                                         \
                                                                               \
} /* namespace detail */ /* GENERATE_METHOD_CHECK */


} /* namespace stream */

#endif
