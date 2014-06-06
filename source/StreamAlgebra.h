#ifndef STREAM_ALGEBRA_H
#define STREAM_ALGEBRA_H

#include "Utility.h"

template<typename T>
stream::Stream<std::result_of_t<std::negate<void>(T&&)>> operator- (stream::Stream<T>&& stream) {
    return stream.map(std::negate<void>());
}

template<typename T>
stream::Stream<decltype(+std::declval<T&&>())> operator+ (stream::Stream<T>&& stream) {
    return stream.map([](T&& x) {
        return +x;
    });
}

template<typename T>
stream::Stream<decltype(!std::declval<T&&>())> operator! (stream::Stream<T>&& stream) {
    return stream.map([](T&& x) {
        return !x;
    });
}

template<typename T>
stream::Stream<decltype(~std::declval<T&&>())> operator~ (stream::Stream<T>&& stream) {
    return stream.map([](T&& x) {
        return ~x;
    });
}

template<typename T>
stream::Stream<decltype(*std::declval<T&&>())> operator* (stream::Stream<T>&& stream) {
    return stream.map([](T&& x) {
        return *x;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::plus<void>(T1&&, T2&&)>>
operator+ (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right), std::plus<void>());
}

template<typename S, typename T>
auto operator+ (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() + right)> {
    return left.map([&right] (S&& value) {
        return value + right;
    });
}

template<typename S, typename T>
auto operator+ (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left + std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left + value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::minus<void>(T1&&, T2&&)>>
operator- (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::minus<void>());
}

template<typename S, typename T>
auto operator- (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() - right)> {
    return left.map([&right] (S&& value) {
        return value - right;
    });
}

template<typename S, typename T>
auto operator- (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left - std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left - value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::multiplies<void>(T1&&, T2&&)>>
operator* (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::multiplies<void>());
}

template<typename S, typename T>
auto operator* (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() * right)> {
    return left.map([&right] (S&& value) {
        return value * right;
    });
}

template<typename S, typename T>
auto operator* (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left * std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left * value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::divides<void>(T1&&, T2&&)>>
operator/ (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::divides<void>());
}

template<typename S, typename T>
auto operator/ (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() / right)> {
    return left.map([&right] (S&& value) {
        return value / right;
    });
}

template<typename S, typename T>
auto operator/ (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left / std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left / value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::modulus<void>(T1&&, T2&&)>>
operator% (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::modulus<void>());
}

template<typename S, typename T>
auto operator% (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() % right)> {
    return left.map([&right] (S&& value) {
        return value % right;
    });
}

template<typename S, typename T>
auto operator% (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left % std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left % value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::equal_to<void>(T1&&, T2&&)>>
operator== (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::equal_to<void>());
}

template<typename S, typename T>
auto operator== (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() == right)> {
    return left.map([&right] (S&& value) {
        return value == right;
    });
}

template<typename S, typename T>
auto operator== (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left == std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left == value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::not_equal_to<void>(T1&&, T2&&)>>
operator!= (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::not_equal_to<void>());
}

template<typename S, typename T>
auto operator!= (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() != right)> {
    return left.map([&right] (S&& value) {
        return value != right;
    });
}

template<typename S, typename T>
auto operator!= (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left != std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left != value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::less<void>(T1&&, T2&&)>>
operator< (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::less<void>());
}

template<typename S, typename T>
auto operator< (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() < right)> {
    return left.map([&right] (S&& value) {
        return value < right;
    });
}

template<typename S, typename T>
auto operator< (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left < std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left < value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::greater<void>(T1&&, T2&&)>>
operator> (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::greater<void>());
}

template<typename S, typename T>
auto operator> (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() > right)> {
    return left.map([&right] (S&& value) {
        return value > right;
    });
}

template<typename S, typename T>
auto operator> (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left > std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left > value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::less_equal<void>(T1&&, T2&&)>>
operator<= (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::less_equal<void>());
}

template<typename S, typename T>
auto operator<= (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() <= right)> {
    return left.map([&right] (S&& value) {
        return value <= right;
    });
}

template<typename S, typename T>
auto operator<= (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left <= std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left <= value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::greater_equal<void>(T1&&, T2&&)>>
operator>= (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::greater_equal<void>());
}

template<typename S, typename T>
auto operator>= (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() >= right)> {
    return left.map([&right] (S&& value) {
        return value >= right;
    });
}

template<typename S, typename T>
auto operator>= (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left >= std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left >= value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::logical_and<void>(T1&&, T2&&)>>
operator&& (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::logical_and<void>());
}

template<typename S, typename T>
auto operator&& (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() && right)> {
    return left.map([&right] (S&& value) {
        return value && right;
    });
}

template<typename S, typename T>
auto operator&& (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left && std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left && value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::logical_or<void>(T1&&, T2&&)>>
operator|| (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::logical_or<void>());
}

template<typename S, typename T>
auto operator|| (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() || right)> {
    return left.map([&right] (S&& value) {
        return value || right;
    });
}

template<typename S, typename T>
auto operator|| (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left || std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left || value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::bit_and<void>(T1&&, T2&&)>>
operator& (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::bit_and<void>());
}

template<typename S, typename T>
auto operator& (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() & right)> {
    return left.map([&right] (S&& value) {
        return value & right;
    });
}

template<typename S, typename T>
auto operator& (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left & std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left & value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::bit_or<void>(T1&&, T2&&)>>
operator| (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::bit_or<void>());
}

template<typename S, typename T>
auto operator| (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() | right)> {
    return left.map([&right] (S&& value) {
        return value | right;
    });
}

template<typename S, typename T>
auto operator| (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left | std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left | value;
    });
}

template<typename T1, typename T2>
stream::Stream<std::result_of_t<std::bit_xor<void>(T1&&, T2&&)>>
operator^ (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(std::forward<stream::Stream<T2>>(right),
                         std::bit_xor<void>());
}

template<typename S, typename T>
auto operator^ (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() ^ right)> {
    return left.map([&right] (S&& value) {
        return value ^ right;
    });
}

template<typename S, typename T>
auto operator^ (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left ^ std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left ^ value;
    });
}

template<typename T1, typename T2>
stream::Stream<decltype(std::declval<T1&&>() << std::declval<T2&&>())>
operator<< (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(right, [](T1& t1, T2& t2) {
        return t1 << t2; });
}

template<typename S, typename T>
auto operator<< (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() << right)> {
    return left.map([&right] (S&& value) {
        return value << right;
    });
}

template<typename S, typename T>
auto operator<< (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left << std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left << value;
    });
}

template<typename T1, typename T2>
stream::Stream<decltype(std::declval<T1&&>() >> std::declval<T2&&>())>
operator>> (stream::Stream<T1>&& left, stream::Stream<T2>&& right) {
    return left.zip_with(right, [](T1& t1, T2& t2) {
        return t1 >> t2; });
}

template<typename S, typename T>
auto operator>> (stream::Stream<S>&& left, const T& right)
        -> stream::Stream<decltype(std::declval<S&&>() >> right)> {
    return left.map([&right] (S&& value) {
        return value >> right;
    });
}

template<typename S, typename T>
auto operator>> (const T& left, stream::Stream<S>&& right)
        -> stream::Stream<decltype(left >> std::declval<S&&>())> {
    return right.map([&left] (S&& value) {
        return left >> value;
    });
}


#endif
