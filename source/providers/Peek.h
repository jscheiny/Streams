#ifndef SCHEINERMAN_STREAM_PROVIDERS_PEEK_H
#define SCHEINERMAN_STREAM_PROVIDERS_PEEK_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename Source, typename Action>
class peek {

public:
    using element = typename Source::element;

    peek(std::unique_ptr<Source> source, Action&& action)
        : source_(std::move(source))
        , action_(action) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (stream_advance(source_)) {
            current_ = source_->get();
            action_(*current_);
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Peek:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Action action_;
    std::shared_ptr<element> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
