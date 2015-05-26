#ifndef SCHEINERMAN_STREAM_PROVIDERS_STATEFUL_H
#define SCHEINERMAN_STREAM_PROVIDERS_STATEFUL_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <list>

namespace stream {
namespace provider {

template<typename Source>
class stateful {

public:
    using element = typename Source::element;

    stateful(std::unique_ptr<Source> source)
        : source_(std::move(source)) {}

    std::shared_ptr<element> get() {
        return *current_;
    }

    bool advance() {
        if (first_) {
            first_ = false;
            while (stream_advance(source_)) {
                state_.push_back(source_->get());
            }
            current_ = state_.begin();
            return current_ != state_.end();
        }
        ++current_;
        return current_ != state_.end();
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "StatePoint:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    using List = std::list<std::shared_ptr<element>>;
    using Iterator = typename List::iterator;

    bool first_ = true;
    std::unique_ptr<Source> source_;
    List state_;
    Iterator current_;
    Iterator end_;

};

} /* namespace provider */
} /* namespace stream */

#endif
