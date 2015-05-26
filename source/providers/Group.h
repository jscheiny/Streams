#ifndef SCHEINERMAN_STREAM_PROVIDERS_GROUP_H
#define SCHEINERMAN_STREAM_PROVIDERS_GROUP_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

namespace detail {

struct IncompleteGroupError {};

template<typename Source>
auto next(std::unique_ptr<Source>& source) {
    if(stream_advance(source)) {
        return source->get();
    }
    throw IncompleteGroupError();
}

template<typename T, size_t N>
struct Grouper {
    using Type = NTuple<T, N>;

    template<typename Source>
    static Type group(std::unique_ptr<Source>& source) {
        auto sub = Grouper<T, N-1>::group(source);
        auto curr = next(source);
        return std::tuple_cat(sub, std::make_tuple<T>(std::move(*curr)));
    }
};

template<typename T>
struct Grouper<T, 3> {
    using Type = NTuple<T, 3>;

    template<typename Source>
    static Type group(std::unique_ptr<Source>& source) {
        auto first = next(source);
        auto second = next(source);
        auto third = next(source);
        return std::make_tuple<T, T, T>(std::move(*first),
                                        std::move(*second),
                                        std::move(*third));
    }
};

template<typename T>
struct Grouper<T, 2> {
    using Type = std::pair<T, T>;

    template<typename Source>
    static Type group(std::unique_ptr<Source>& source) {
        auto first = next(source);
        auto second = next(source);
        return std::make_pair<T, T>(std::move(*first),
                                    std::move(*second));
    }
};

template<typename T, size_t N>
using GroupResult = typename detail::Grouper<T, N>::Type;

} /* namespace detail */

template<typename Source, size_t N>
class group {
    static_assert(N >= 2, "Cannot group stream into chunks of less than size 2.");

private:
    using source_elem = typename Source::element;

public:
    using element = detail::GroupResult<source_elem, N>;

    group(std::unique_ptr<Source> source)
        : source_(std::move(source)) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        try {
            auto group = detail::Grouper<source_elem, N>::group(source_);
            current_ = std::make_shared<element>(std::move(group));
            return true;
        } catch (detail::IncompleteGroupError& err) {
            return false;
        }
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Grouped[" << N << "]:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    std::shared_ptr<element> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
