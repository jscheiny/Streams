#ifndef SCHEINERMAN_STREAM_PROVIDERS_PARTIAL_SUM_H
#define SCHEINERMAN_STREAM_PROVIDERS_PARTIAL_SUM_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename Source, typename Adder>
class partial_sum {

public:
    using element = typename Source::element;

    partial_sum(std::unique_ptr<Source> source, Adder&& add)
        : source_(std::move(source))
        , add_(add) {}

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

        if (stream_advance(source_)) {
            current_ = std::make_shared<element>(add_(
                std::move(*current_),
                std::move(*source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "PartialSum:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Adder add_;
    std::shared_ptr<element> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
