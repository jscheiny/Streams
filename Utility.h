#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <tuple>

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
std::unique_ptr<T> move_unique(T&& t) {
    return make_unique<T>(std::move(t));
}

template<typename Transform, typename... Arg> using ReturnType =
    decltype(std::declval<Transform>()(std::declval<Arg>()...));

template<typename T, template<typename> class Pointer, typename Compare,
         bool Reverse = false>
class ComparePtrWrapper {
public:
    ComparePtrWrapper(Compare&& comparator) : comparator_(comparator) {}

    bool operator() (const Pointer<T>& left, const Pointer<T>& right) {
        return comparator_(*left, *right);
    }

private:
    Compare comparator_;
};

template<typename T, template<typename> class Pointer, typename Compare>
class ComparePtrWrapper<T, Pointer, Compare, true> {
public:
    ComparePtrWrapper(Compare&& comparator) : comparator_(comparator) {}

    bool operator() (const Pointer<T>& left, const Pointer<T>& right) {
        return comparator_(*right, *left);
    }

private:
    Compare comparator_;
};


template<size_t index, size_t last, typename... Args> struct PrintTuple;

template<typename... Args>
std::ostream& operator<< (std::ostream& os, const std::tuple<Args...>& tuple) {
    PrintTuple<0, std::tuple_size<std::tuple<Args...>>::value - 1, Args...>::print(os, tuple);
    return os;
}

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



#endif
