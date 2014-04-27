#ifndef UTILITY_H
#define UTILITY_H

#include "Utility-Impl.h"

#include <iostream>
#include <tuple>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
struct Minus {
    auto operator() (T& lhs, T& rhs) -> decltype(lhs - rhs) {
        return lhs - rhs;
    }
};

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

template<typename Function>
SplattedFunction<Function> splat(Function&& function) {
    return SplattedFunction<Function>(std::forward<Function>(function));
}


#endif
