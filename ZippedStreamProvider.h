#ifndef ZIPPED_STREAM_PROVIDER
#define ZIPPED_STREAM_PROVIDER

#include "StreamProvider.h"

#include <tuple>

template<typename L, typename R>
struct ZipResult {
    using Type = std::tuple<L, R>;

    static Type zip(L&& a, R&& b) {
        return std::make_tuple(std::move(a), std::move(b));
    }
};

template<typename L, typename... RArgs>
struct ZipResult<L, std::tuple<RArgs...>> {
    using Type = std::tuple<L, RArgs...>;

    static Type zip(L&& a, std::tuple<RArgs...>&& b) {
        return std::tuple_cat(std::make_tuple(a), std::move(b));
    }
};


template<typename... LArgs, typename R>
struct ZipResult<std::tuple<LArgs...>, R> {
    using Type = std::tuple<LArgs..., R>;

    static Type zip(std::tuple<LArgs...>&& a, R&& b) {
        return std::tuple_cat(std::move(a), std::make_tuple(b));
    }
};

template<typename... LArgs, typename... RArgs>
struct ZipResult<std::tuple<LArgs...>, std::tuple<RArgs...>> {
    using Type = std::tuple<LArgs..., RArgs...>;

    static Type zip(std::tuple<LArgs...>&& a, std::tuple<RArgs...>&& b) {
        return std::tuple_cat(std::move(a), std::move(b));
    }
};

template<typename L, typename R, template<typename> class Pointer,
         typename Value = typename ZipResult<L, R>::Type>
class ZippedStreamProvider : public StreamProvider<Value, Pointer> {

public:
    using Zipper = ZipResult<L, R>;

    ZippedStreamProvider(StreamProviderPtr<L, Pointer> left_source,
                         StreamProviderPtr<R, Pointer> right_source)
        : left_source_(std::move(left_source)),
          right_source_(std::move(right_source)) {}

    Pointer<Value> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(left_source_->advance() && right_source_->advance()) {
            current_ = move_unique(Zipper::zip(
                std::move(*left_source_->get()),
                std::move(*right_source_->get())));
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<L, Pointer> left_source_;
    StreamProviderPtr<R, Pointer> right_source_;
    Pointer<Value> current_;
};

#endif
