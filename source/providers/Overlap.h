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

template<typename T, size_t N>
class Overlap : public StreamProvider<NTuple<T, N>> {

private:
    using Tuple = NTuple<T, N>;

public:
    Overlap(StreamProviderPtr<T> source) : source_(std::move(source)) {}

    std::shared_ptr<Tuple> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            try {
                current_ = std::make_shared<Tuple>(detail::Grouper<T, N>::group(source_));
                return true;
            } catch(detail::IncompleteGroupError& e) {
                return false;
            }
        }

        if(source_->advance()) {
            current_ = std::make_shared<Tuple>(
                rotate(std::move(*current_), std::move(*source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Overlapped[" << N << "]:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    bool first_ = true;
    StreamProviderPtr<T> source_;
    std::shared_ptr<Tuple> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
