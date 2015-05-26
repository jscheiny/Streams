#ifndef SCHEINERMAN_STREAM_PROVIDERS_SORT_H
#define SCHEINERMAN_STREAM_PROVIDERS_SORT_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <vector>
#include <queue>

namespace stream {
namespace provider {

template<typename Source, typename RawCompare>
class sort {

public:
    using element = typename Source::element;

    sort(std::unique_ptr<Source> source, RawCompare&& comparator)
        : source_(std::move(source))
        , sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (first_) {
            while (stream_advance(source_)) {
                sorted_.push(source_->get());
            }
            first_ = false;
        }
        if (sorted_.empty()) {
            current_.reset();
            return false;
        }
        current_ = sorted_.top();
        sorted_.pop();
        return true;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Sort:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    using PointerCompare = ComparePtrWrapper<element, RawCompare, true>;
    using PointerQueue = std::priority_queue<std::shared_ptr<element>,
                                             std::vector<std::shared_ptr<element>>,
                                             PointerCompare>;

    std::unique_ptr<Source> source_;
    PointerQueue sorted_;
    std::shared_ptr<element> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
