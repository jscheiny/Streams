#ifndef SCHEINERMAN_STREAM_PROVIDERS_OVERLAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_OVERLAP_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename... Args, typename T, size_t... I, size_t N = sizeof...(Args)>
NTuple<T, N> rotate_impl(std::tuple<Args...>&& tuple, T&& last, std::index_sequence<I...>) {
    return {std::forward<T>(std::get<I>(tuple))..., std::forward<T>(last)};
}

template<typename... Args, typename T = Head<Args...>, size_t N = sizeof...(Args)>
NTuple<T, N> rotate(std::tuple<Args...>&& tuple, T&& last) {
    return rotate_impl(std::forward<std::tuple<Args...>>(tuple),
                       std::forward<T>(last),
                       make_1based_sequence<size_t, N>());
}

template<typename Source, size_t N>
class overlap {

private:
    using source_elem = typename Source::element;

public:
    using element = NTuple<source_elem, N>;

    overlap(std::unique_ptr<Source> source)
        : source_(std::move(source)) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (first_) {
            first_ = false;
            try {
                current_ = std::make_shared<element>(detail::Grouper<source_elem, N>::group(source_));
                return true;
            } catch(detail::IncompleteGroupError& e) {
                return false;
            }
        }

        if(stream_advance(source_)) {
            current_ = std::make_shared<element>(
                rotate(std::move(*current_), std::move(*source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Overlapped[" << N << "]:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    bool first_ = true;
    std::unique_ptr<Source> source_;
    std::shared_ptr<element> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
