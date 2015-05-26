#ifndef SCHEINERMAN_STREAM_PROVIDERS_SLICE_H
#define SCHEINERMAN_STREAM_PROVIDERS_SLICE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename Source>
class slice {

public:
    using element = typename Source::element;

    slice(std::unique_ptr<Source> source, size_t start, size_t end, size_t increment, bool no_end)
        : source_(std::move(source))
        , start_(start)
        , end_(end)
        , increment_(increment)
        , no_end_(no_end) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (index_ < start_) {
            for (; index_ <= start_; index_++) {
                if (stream_advance(source_)) {
                    current_ = source_->get();
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if (no_end_ or index_ + increment_ <= end_) {
            for (size_t k = 0; k < increment_; k++) {
                index_++;
                if (stream_advance(source_)) {
                    current_ = source_->get();
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Slice[" << start_ << ", "
                       << (no_end_ ? -1 : static_cast<long>(end_)) << ", "
                       << increment_ <<  "]:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    bool first_ = true;
    std::unique_ptr<Source> source_;
    std::shared_ptr<element> current_;
    size_t index_ = 0;
    size_t start_;
    size_t end_;
    size_t increment_;
    bool no_end_;

};

} /* namespace provider */
} /* namespace stream */

#endif
