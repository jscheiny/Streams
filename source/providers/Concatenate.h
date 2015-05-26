#ifndef SCHEINERMAN_STREAM_PROVIDERS_CONCATENATE_H
#define SCHEINERMAN_STREAM_PROVIDERS_CONCATENATE_H

#include "StreamProvider.h"

#include <list>

namespace stream {
namespace provider {

template<typename FirstSource, typename SecondSource>
class concatenate {

private:
    using first_elem = typename FirstSource::element;
    using second_elem = typename SecondSource::element;
    static_assert(std::is_same<first_elem, second_elem>::value,
        "Cannot concatenate streams of different types.");

public:
    using element = first_elem;

    concatenate(std::unique_ptr<FirstSource> first, std::unique_ptr<SecondSource> second)
        : first_source_(std::move(first))
        , second_source_(std::move(second)) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (!first_depleted_) {
            if (stream_advance(first_source_)) {
                current_ = first_source_->get();
                return true;
            }
            first_depleted_ = true;
        }

        if (stream_advance(second_source_)) {
            current_ = second_source_->get();
            return true;
        }

        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Concatenation:\n";
        return first_source_->print(os, indent + 1)
             + second_source_->print(os, indent + 1);
    }

private:
    std::unique_ptr<FirstSource> first_source_;
    std::unique_ptr<SecondSource> second_source_;
    std::shared_ptr<element> current_;
    bool first_depleted_ = false;
};

} /* namespace provider */
} /* namespace stream */

#endif
