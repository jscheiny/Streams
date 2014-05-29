#ifndef UTILITY_H
#define UTILITY_H

#include "Utility-Impl.h"

#include <iostream>
#include <iterator>
#include <tuple>

template<typename T, bool IsClass> class StreamImpl;

template<typename T>
using Stream = StreamImpl<T, std::is_class<T>::value>;

template<typename T> struct StreamIdentifier {  using Type = void; };
template<typename T> struct StreamIdentifier<Stream<T>> { using Type = T; };

template<typename S> using StreamType = typename StreamIdentifier<S>::Type;

template<typename T> struct IsStream : public std::integral_constant<bool, false> {};
template<typename T> struct IsStream<Stream<T>> : public std::integral_constant<bool, true> {};

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

template<typename Function>
InvertedPredicate<Function> not_(Function&& fn) {
    return {std::forward<Function>(fn)};
}

template<typename T>
using RemoveRef = typename std::remove_reference<T>::type;

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


#endif
