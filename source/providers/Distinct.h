#ifndef SCHEINERMAN_STREAM_PROVIDERS_DISTINCT_H
#define SCHEINERMAN_STREAM_PROVIDERS_DISTINCT_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <set>

namespace stream {
namespace provider {

template<typename Source, typename RawCompare>
class distinct {

public:
    using element = typename Source::element;

    distinct(std::unique_ptr<Source> source, RawCompare&& comparator)
        : source_(std::move(source))
        , sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (first_) {
            while (stream_advance(source_)) {
                sorted_.insert(source_->get());
            }
            first_ = false;
        }
        if (sorted_.empty()) {
            current_.reset();
            return false;
        }
        auto itr = sorted_.begin();
        current_ = *itr;
        sorted_.erase(itr);
        return true;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Distinct:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    using PointerCompare = ComparePtrWrapper<element, RawCompare>;
    using PointerQueue = std::set<std::shared_ptr<element>, PointerCompare>;

    std::unique_ptr<Source> source_;
    PointerQueue sorted_;
    std::shared_ptr<element> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
