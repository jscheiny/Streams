#ifndef SCHEINERMAN_STREAM_PROVIDERS_RECURRENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_RECURRENCE_H

#include "StreamProvider.h"

#include <array>

namespace stream {
namespace provider {

namespace detail {

template<typename Function, typename Array, size_t... I>
inline auto apply_impl(Function&& function, Array& args, std::index_sequence<I...>) {
    return function(std::get<I>(args)...);
}

template<typename Function, typename T, size_t N>
inline auto apply(Function&& function, std::array<T, N>& args) {
    return apply_impl(std::forward<Function>(function), args,
                      std::make_index_sequence<N>());
}

template<typename T, size_t N, size_t... I>
std::array<T, N> rotate_impl(std::array<T, N>&& array, const T& last, std::index_sequence<I...>) {
    return {{std::forward<T>(std::get<I>(array))..., last}};
}

template<typename T, size_t N>
std::array<T, N> rotate(std::array<T, N>&& array, const T& last) {
    return rotate_impl(std::forward<std::array<T, N>>(array), last,
                       make_1based_sequence<size_t, N>());
}

} /* namespace detail */

template<typename T, size_t Order, typename Function>
class Recurrence : public StreamProvider<T> {
public:
    Recurrence(std::array<T, Order>&& arguments, Function&& function)
        : arguments_(arguments), function_(function) {}

    std::shared_ptr<T> get() override {
        return std::make_shared<T>(next_);
    }

    bool advance_impl() override {
        if(index_ < Order) {
            next_ = arguments_[index_++];
            return true;
        }
        next_ = detail::apply(function_, arguments_);
        arguments_ = detail::rotate(std::move(arguments_), next_);
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[recurrence stream]\n";
        return PrintInfo::Source();
    }

private:
    size_t index_ = 0;
    std::array<T, Order> arguments_;
    Function function_;
    T next_;
};

}; /* namespace provider */
}; /* namespace stream */

#endif
