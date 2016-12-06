#ifndef SCHEINERMAN_STREAM_UTILITY_H
#define SCHEINERMAN_STREAM_UTILITY_H

#include "StreamForward.h"
#include "UtilityImpl.h"

#include <iostream>
#include <iterator>
#include <tuple>

namespace stream {

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
inline auto apply_tuple(Function&& function, const std::tuple<Types...>& tuple)
        -> decltype(function(std::declval<Types>()...));

template<typename Function, typename L, typename R>
inline auto apply_tuple(Function&& function, const std::pair<L, R>& pair)
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

} /* namespace stream */

#endif
