#ifndef SCHEINERMAN_STREAM_STREAM_FORWARD_H
#define SCHEINERMAN_STREAM_STREAM_FORWARD_H

#include <type_traits>

namespace stream {

template<typename Provider> class Stream;

namespace detail {

template<typename Stream>
struct stream_info_impl {
    constexpr static bool value = false;
    using type = void;
};

template<typename Provider>
struct stream_info_impl<Stream<Provider>> {
    constexpr static bool value = true;
    using type = typename Provider::element;
};

} /* namespace detail */

template<typename S>
constexpr bool is_stream = detail::stream_info_impl<std::remove_reference_t<S>>::value;

template<typename S>
using stream_type = typename detail::stream_info_impl<std::remove_reference_t<S>>::type;

} /* namespace stream */


#endif
