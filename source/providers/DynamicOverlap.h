#ifndef SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_OVERLAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_OVERLAP_H

#include "StreamProvider.h"

#include <deque>

namespace stream {
namespace provider {

template<typename Source>
class dynamic_overlap {

private:
    using source_elem = typename Source::element;

public:
    using element = std::deque<source_elem>;

    dynamic_overlap(std::unique_ptr<Source> source, size_t N)
        : source_(std::move(source))
        , N_(N) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (first_) {
            first_ = false;
            current_ = std::make_shared<element>();
            for (size_t i = 0; i < N_; i++) {
                if (stream_advance(source_)) {
                    current_->emplace_back(std::move(*source_->get()));
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if (stream_advance(source_)) {
            current_ = std::make_shared<element>(*current_);
            current_->pop_front();
            current_->emplace_back(std::move(*source_->get()));
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Overlapped[" << N_ << "]:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    bool first_ = true;
    std::unique_ptr<Source> source_;
    std::shared_ptr<element> current_;
    const size_t N_;

};

} /* namespace provider */
} /* namespace stream */

#endif

