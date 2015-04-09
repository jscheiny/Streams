#ifndef SCHEINERMAN_STREAM_STREAM_FORWARD_H
#define SCHEINERMAN_STREAM_STREAM_FORWARD_H

#include <type_traits>

namespace stream {

template<typename T> class Stream;

namespace detail {

template<typename T> struct StreamIdentifier { using type = void; };
template<typename T> struct StreamIdentifier<Stream<T>> { using type = T; };

} /* namespace detail */

template<typename S> using StreamType =
    typename detail::StreamIdentifier<std::remove_reference_t<S>>::type;

} /* namespace stream */


#endif
