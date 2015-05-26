#ifndef SCHEINERMAN_STREAM_PROVIDERS_DROP_WHILE_H
#define SCHEINERMAN_STREAM_PROVIDERS_DROP_WHILE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename Source, typename Predicate>
class drop_while {

public:
    using element = typename Source::element;

    drop_while(std::unique_ptr<Source> source, Predicate&& predicate)
        : source_(std::move(source))
        , predicate_(predicate) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (!dropped_) {
            dropped_ = true;
            while (stream_advance(source_)) {
                current_ = source_->get();
                if (!predicate_(*current_)) {
                    return true;
                }
            }
            current_.reset();
            return false;
        }
        if (stream_advance(source_)) {
            current_ = source_->get();
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "drop_while:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Predicate predicate_;
    std::shared_ptr<element> current_;
    bool dropped_ = false;
};

} /* namespace provider */
} /* namespace stream */

#endif
