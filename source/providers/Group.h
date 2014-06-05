#ifndef PROVIDERS_GROUP_H
#define PROVIDERS_GROUP_H

#include "StreamProvider.h"

#include "../Utility.h"

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
private:
    using SubType = typename Grouper<T, N-1>::Type;
public:
    using Type = decltype(std::tuple_cat(std::declval<SubType>()),
                                         std::declval<std::tuple<T>>());

    static Type group(StreamProviderPtr<T>& source) {
        auto sub = Grouper<T, N-1>::group(source);
        auto curr = next(source);
        return std::tuple_cat(sub, std::make_tuple<T>(std::move(*curr)));
    }
};

template<typename T>
struct Grouper<T, 3> {
    using Type = std::tuple<T, T, T>;

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

} /*detail*/

template<typename T, size_t N>
using GroupResult = typename detail::Grouper<T, N>::Type;

template<typename T, size_t N>
class GroupedStreamProvider : public StreamProvider<GroupResult<T, N>> {
    static_assert(N >= 2, "Cannot group stream into chunks of less than size 2.");

public:
    using GroupType = GroupResult<T, N>;

    GroupedStreamProvider(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

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

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "Grouped[" << N << "]:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<T> source_;
    std::shared_ptr<GroupType> current_;

};

#endif
