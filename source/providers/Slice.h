#ifndef SCHEINERMAN_STREAM_PROVIDERS_SLICE_H
#define SCHEINERMAN_STREAM_PROVIDERS_SLICE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T>
class Slice : public StreamProvider<T> {

public:
    Slice(StreamProviderPtr<T> source, size_t start, size_t end, size_t increment, bool no_end_)
        : source_(std::move(source)),
          start_(start),
          end_(end),
          increment_(increment),
          no_end_(no_end_) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(index_ < start_) {
            for(; index_ <= start_; index_++) {
                if(source_->advance()) {
                    current_ = source_->get();
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if(no_end_ || index_ + increment_ <= end_) {
            for(size_t k = 0; k < increment_; k++) {
                index_++;
                if(source_->advance()) {
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

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Slice[" << start_ << ", "
                       << (no_end_ ? -1 : static_cast<long>(end_)) << ", "
                       << increment_ <<  "]:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    bool first_ = true;
    StreamProviderPtr<T> source_;
    std::shared_ptr<T> current_;
    size_t index_ = 0;
    size_t start_;
    size_t end_;
    size_t increment_;
    bool no_end_;


};

} /* namespace provider */
} /* namespace stream */

#endif
