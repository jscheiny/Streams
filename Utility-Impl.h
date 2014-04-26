#ifndef UTILITY_IMPL_H
#define UTILITY_IMPL_H

#include <iostream>
#include <tuple>

template<typename Function, typename... Args> using ReturnType =
    decltype(std::declval<Function>()(std::declval<Args>()...));

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
auto apply_tuple(Function&& function, const std::tuple<Types...>& tuple)
        -> decltype(function(std::declval<Types>()...)) {

    using Return = decltype(function(std::declval<Types>()...));
    return SplatTuple<Return, Function, 0, sizeof...(Types),
                      TupleWrapper<Types...>>
        ::splat(std::forward<Function>(function), tuple);

}

template<typename Function>
struct SplattedFunction {

public:
    SplattedFunction(Function&& function) : function_(function) {}

    template<typename... Args>
    auto operator() (const std::tuple<Args...>& tuple)
            -> ReturnType<Function, Args...> {

        return apply_tuple(function_, tuple);
    }

private:
    Function function_;

};

#endif
