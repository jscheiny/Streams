#ifndef SCHEINERMAN_STREAM_PROVIDERS_GROUP_H
#define SCHEINERMAN_STREAM_PROVIDERS_GROUP_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

namespace detail {

struct IncompleteGroupError {};

template<typename T>
std::shared_ptr<T> next(StreamProviderPtr<T>& source) {
    if(source->advance()) {
        return source->get();
    }
    throw IncompleteGroupError();
}

template<typename T, size_t N>
struct Grouper {
    using Type = NTuple<T, N>;

    static Type group(StreamProviderPtr<T>& source) {
        auto sub = Grouper<T, N-1>::group(source);
        auto curr = next(source);
        return std::tuple_cat(sub, std::make_tuple<T>(std::move(*curr)));
    }
};

template<typename T>
struct Grouper<T, 3> {
    using Type = NTuple<T, 3>;

    static Type group(StreamProviderPtr<T>& source) {
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

    static Type group(StreamProviderPtr<T>& source) {
        auto first = next(source);
        auto second = next(source);
        return std::make_pair<T, T>(std::move(*first),
                                    std::move(*second));
    }
};

template<typename T, size_t N>
using GroupResult = typename detail::Grouper<T, N>::Type;

} /* namespace detail */

template<typename T, size_t N>
class Group : public StreamProvider<detail::GroupResult<T, N>> {
    static_assert(N >= 2, "Cannot group stream into chunks of less than size 2.");

public:
    using GroupType = detail::GroupResult<T, N>;

    Group(StreamProviderPtr<T> source) : source_(std::move(source)) {}

    std::shared_ptr<GroupType> get() override {
        return current_;
    }

    bool advance_impl() override {
        try {
            auto group = detail::Grouper<T, N>::group(source_);
            current_ = std::make_shared<GroupType>(std::move(group));
            return true;
        } catch(detail::IncompleteGroupError& err) {
            return false;
        }
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Grouped[" << N << "]:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    std::shared_ptr<GroupType> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
