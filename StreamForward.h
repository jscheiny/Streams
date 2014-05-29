#ifndef STREAM_FORWARD_H
#define STREAM_FORWARD_H

#include <type_traits>

enum class StreamTag {
    Common,
    Class,
    Bool
};

template<typename T, StreamTag Tag> class StreamImpl;

template<typename T> struct ResolveTag : public std::integral_constant<StreamTag,
    std::is_same<T,bool>::value ? StreamTag::Bool :
        (std::is_class<T>::value ? StreamTag::Class :
                                    StreamTag::Common)> {};

template<typename T>
using Stream = StreamImpl<T, ResolveTag<T>::value>;

template<typename T> struct StreamIdentifier {  using Type = void; };
template<typename T> struct StreamIdentifier<Stream<T>> { using Type = T; };

template<typename S> using StreamType = typename StreamIdentifier<S>::Type;

template<typename T> struct IsStream : public std::false_type {};
template<typename T> struct IsStream<Stream<T>> : public std::true_type {};


#endif