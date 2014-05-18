#ifndef SLICED_STREAM_PROVIDER_H
#define SLICED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class SlicedStreamProvider : public StreamProvider<T> {

public:
    SlicedStreamProvider(StreamProviderPtr<T> source,
                         size_t start, size_t end, size_t increment, bool no_end_)
        : source_(std::move(source)),
          start_(start),
          end_(end),
          increment_(increment),
          no_end_(no_end_) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(index_ < start_) {
            for(; index_ < start_; index_++) {
                if(source_->advance()) {
                    current_ = source_->get();
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if(no_end_ || index_ + increment_ < end_) {
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

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "Slice[" << start_ << ", " 
                       << (no_end_ ? -1 : end_) << ", "
                       << increment_ <<  "]:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
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

#endif
