#ifndef SCHEINERMAN_STREAM_PROVIDERS_ZIP_H
#define SCHEINERMAN_STREAM_PROVIDERS_ZIP_H

#include "StreamProvider.h"

#include <tuple>
#include <type_traits>

namespace stream {
namespace provider {

namespace detail {

template<typename L, typename R>
struct ZipperImpl {
    using Type = std::tuple<L, R>;

    static Type zip(L&& a, R&& b) {
        return std::make_tuple<L, R>(std::move(a), std::move(b));
    }
};

template<typename L, typename... RArgs>
struct ZipperImpl<L, std::tuple<RArgs...>> {
    using Type = std::tuple<L, RArgs...>;

    static Type zip(L&& a, std::tuple<RArgs...>&& b) {
        return std::tuple_cat(std::make_tuple<L>(std::move(a)), std::move(b));
    }
};


template<typename... LArgs, typename R>
struct ZipperImpl<std::tuple<LArgs...>, R> {
    using Type = std::tuple<LArgs..., R>;

    static Type zip(std::tuple<LArgs...>&& a, R&& b) {
        return std::tuple_cat(std::move(a), std::make_tuple<R>(std::move(b)));
    }
};

template<typename... LArgs, typename... RArgs>
struct ZipperImpl<std::tuple<LArgs...>, std::tuple<RArgs...>> {
    using Type = std::tuple<LArgs..., RArgs...>;

    static Type zip(std::tuple<LArgs...>&& a, std::tuple<RArgs...>&& b) {
        return std::tuple_cat(std::move(a), std::move(b));
    }
};

struct Zipper {
    template<typename L, typename R>
    auto operator() (L&& left, R&& right) {
        using Left = typename std::decay<L>::type;
        using Right = typename std::decay<R>::type;
        return detail::ZipperImpl<Left, Right>::zip(std::forward<Left>(left),
                                                    std::forward<Right>(right));
    }
};

} /* namespace detail */

template<typename LeftSource, typename RightSource, typename Function>
class zip {

private:
    using left_elem = typename LeftSource::element;
    using right_elem = typename RightSource::element;

public:
    using element = std::result_of_t<Function(left_elem&&, right_elem&&)>;
    static_assert(!std::is_void<element>::value,
        "Return type of the zipping function cannot be void.");

    zip(std::unique_ptr<LeftSource> left_source,
        std::unique_ptr<RightSource> right_source,
        Function&& zipper)
          : left_source_(std::move(left_source))
          , right_source_(std::move(right_source))
          , zipper_(zipper) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (stream_advance(left_source_) && stream_advance(right_source_)) {
            current_ = std::make_shared<element>(
                zipper_(std::move(*left_source_->get()),
                        std::move(*right_source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Zip:\n";
        return left_source_->print(os, indent + 1)
             + right_source_->print(os, indent + 1);
    }

private:
    std::unique_ptr<LeftSource> left_source_;
    std::unique_ptr<RightSource> right_source_;
    std::shared_ptr<element> current_;
    Function zipper_;
};

} /* namespace provider */
} /* namespace stream */

#endif
