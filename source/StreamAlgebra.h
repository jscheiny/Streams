#ifndef SCHEINERMAN_STREAM_STREAM_ALGEBRA_H
#define SCHEINERMAN_STREAM_STREAM_ALGEBRA_H

#include "Utility.h"

#define STREAM_OP_STREAM(Op, Function) \
    template<typename T1, typename T2> \
    stream::Stream<std::result_of_t< Function (T1&&, T2&&)>> \
    operator Op (stream::Stream<T1>&& left, stream::Stream<T2>&& right) { \
        return left | stream::op::zip_with(std::forward<stream::Stream<T2>>(right), \
                                           Function{}); \
    }

#define STREAM_OP_VALUE(Op) \
    template<typename S, typename T> \
    auto operator Op (stream::Stream<S>&& left, const T& right) \
            -> stream::Stream<decltype(std::declval<S&&>() Op right)> { \
        return left | stream::op::map_([right] (S&&value) { \
            return value Op right; \
        }); \
    }

#define VALUE_OP_STREAM(Op) \
    template<typename S, typename T> \
    auto operator Op (const T& left, stream::Stream<S>&& right) \
            -> stream::Stream<decltype(left Op std::declval<S&&>())> { \
        return right | stream::op::map_([left] (S&& value) { \
            return left Op value; \
        }); \
    }

#define BINARY_OPERATOR(Op, Function) \
    STREAM_OP_STREAM(Op, Function) \
    STREAM_OP_VALUE(Op) \
    VALUE_OP_STREAM(Op) \

#define UNARY_OPERATOR(Op) \
    template<typename T> \
    stream::Stream<decltype( Op std::declval<T&&>())> operator Op (stream::Stream<T>&& stream) { \
        return stream | stream::op::map_([](T&& x) { return Op x; }); \
    }


struct ShiftLeft {
    template<typename L, typename R>
    constexpr auto operator()(L&& left, R&& right) const
        -> decltype(std::forward<L>(left) << std::forward<R>(right)) {
        return std::forward<L>(left) << std::forward<R>(right);
    }
};

struct ShiftRight {
    template<typename L, typename R>
    constexpr auto operator()(L&& left, R&& right) const
        -> decltype(std::forward<L>(left) >> std::forward<R>(right)) {
        return std::forward<L>(left) >> std::forward<R>(right);
    }
};


UNARY_OPERATOR(-);
UNARY_OPERATOR(+);
UNARY_OPERATOR(!);
UNARY_OPERATOR(~);
UNARY_OPERATOR(*);

BINARY_OPERATOR(+, std::plus<void>);
BINARY_OPERATOR(-, std::minus<void>);
BINARY_OPERATOR(*, std::multiplies<void>);
BINARY_OPERATOR(/, std::divides<void>);
BINARY_OPERATOR(%, std::modulus<void>);

BINARY_OPERATOR(==, std::equal_to<void>);
BINARY_OPERATOR(!=, std::not_equal_to<void>);
BINARY_OPERATOR(<,  std::less<void>);
BINARY_OPERATOR(>,  std::greater<void>);
BINARY_OPERATOR(<=, std::less_equal<void>);
BINARY_OPERATOR(>=, std::greater_equal<void>);

BINARY_OPERATOR(&&, std::logical_and<void>);
BINARY_OPERATOR(||, std::logical_or<void>);

BINARY_OPERATOR(&,  std::bit_and<void>);
BINARY_OPERATOR(|,  std::bit_or<void>);
BINARY_OPERATOR(^,  std::bit_xor<void>);
BINARY_OPERATOR(<<, ShiftLeft);
BINARY_OPERATOR(>>, ShiftRight);

#undef STREAM_OP_STREAM
#undef STREAM_OP_VALUE
#undef VALUE_OP_STREAM
#undef BINARY_OPERATOR
#undef UNARY_OPERATOR

#endif
