#ifndef SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DISTINCT_H
#define SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DISTINCT_H

#include "StreamProvider.h"
#include "../Utility.h"

namespace stream {
namespace provider {

template<typename Source, typename Equal>
class adjacent_distinct {

public:
    using element = typename Source::element;

    adjacent_distinct(std::unique_ptr<Source> source, Equal&& equal)
        : source_(std::move(source))
        , equal_(std::forward<Equal>(equal_)) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (first_) {
            first_ = false;
            if (stream_advance(source_)) {
                current_ = source_->get();
                return true;
            }
            return false;
        }

        while (stream_advance(source_)) {
            auto next = source_->get();
            if (!equal_(*current_, *next)) {
                current_ = next;
                return true;
            }
        }

        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "AdjacentDistinct:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Equal equal_;
    std::shared_ptr<element> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
