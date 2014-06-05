#ifndef STREAM_FORWARD_H
#define STREAM_FORWARD_H

#include <type_traits>

namespace stream {

enum StreamTag {
    Common  = 0,
    Class   = 1,
    Bool    = 2,
    Number  = 4
};

template<typename T, int Tags> class StreamImpl;

namespace detail {

template<typename T> struct is_class_pointer
    : public std::integral_constant<bool, std::is_class<T>::value> {};
template<typename T> struct is_class_pointer<T*>
    : public std::integral_constant<bool, std::is_class<T>::value> {};

template<typename Value, StreamTag Tag> struct Predicate
    : public std::integral_constant<bool, Value::value> {
    static constexpr StreamTag tag = Tag;
};

template<typename T> struct ClassTag
    : public Predicate<is_class_pointer<T>,          Class>   {};
template<typename T> struct BoolTag
    : public Predicate<std::is_convertible<T, bool>, Bool>    {};
template<typename T> struct NumberTag
    : public Predicate<std::is_arithmetic<T>,        Number>  {};

template<template<typename> class Predicate, typename T>
struct EvaluateTag : public std::integral_constant<StreamTag,
    Predicate<T>::value ? Predicate<T>::tag : Common> {};

template<typename T>
struct ResolveTag : public std::integral_constant<int,
    EvaluateTag<ClassTag,   T> ::value |
    EvaluateTag<BoolTag,    T> ::value |
    EvaluateTag<NumberTag,  T> ::value > {};

} /* namespace detail */

template<typename T>
using Stream = StreamImpl<T, detail::ResolveTag<T>::value>;

namespace detail {

template<typename T> struct StreamIdentifier {  using Type = void; };
template<typename T> struct StreamIdentifier<Stream<T>> { using Type = T; };

template<typename S> using StreamType = typename StreamIdentifier<S>::Type;

} /* namespace detail */

} /* namespace stream */


#endif