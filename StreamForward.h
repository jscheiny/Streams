#ifndef STREAM_FORWARD_H
#define STREAM_FORWARD_H

#include <type_traits>

enum StreamTag {
    Common  = 0,
    Class   = 1,
    Pointer = 2,
    Bool    = 4,
    Number  = 8
};

template<typename T, int Tags> class StreamImpl;

template<typename T> struct is_class_pointer : public std::false_type {};
template<typename T> struct is_class_pointer<T*>
    : public std::integral_constant<bool, std::is_class<T>::value> {};

template<typename Value, StreamTag Tag> struct Predicate
    : public std::integral_constant<bool, Value::value> {
    static constexpr StreamTag tag = Tag;
};

template<typename T> struct ClassTag
    : public Predicate<std::is_class<T>,             Class>   {};
template<typename T> struct PointerTag
    : public Predicate<is_class_pointer<T>,          Pointer> {};
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
    EvaluateTag<PointerTag, T> ::value |
    EvaluateTag<BoolTag,    T> ::value |
    EvaluateTag<NumberTag,  T> ::value > {};

template<typename T>
using Stream = StreamImpl<T, ResolveTag<T>::value>;

template<typename T> struct StreamIdentifier {  using Type = void; };
template<typename T> struct StreamIdentifier<Stream<T>> { using Type = T; };

template<typename S> using StreamType = typename StreamIdentifier<S>::Type;

template<typename T> struct IsStream : public std::false_type {};
template<typename T> struct IsStream<Stream<T>> : public std::true_type {};


#endif