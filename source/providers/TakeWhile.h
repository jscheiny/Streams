#ifndef SCHEINERMAN_STREAM_PROVIDERS_TAKE_WHILE_H
#define SCHEINERMAN_STREAM_PROVIDERS_TAKE_WHILE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename Source, typename Predicate>
class take_while {

public:
    using element = typename Source::element;

    take_while(std::unique_ptr<Source> source, Predicate&& predicate)
        : source_(std::move(source))
        , predicate_(predicate) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (stream_advance(source_)) {
            current_ = source_->get();
            if (!predicate_(*current_)) {
                current_.reset();
                return false;
            }
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "take_while:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Predicate predicate_;
    std::shared_ptr<element> current_;
};

} /* namespace provider */
} /* namespace stream */

#endif
