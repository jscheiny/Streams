#ifndef SLICED_STREAM_PROVIDER_H
#define SLICED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class SlicedStreamProvider : public StreamProvider<T> {

public:
    SlicedStreamProvider(StreamProviderPtr<T> source,
                         size_t start, size_t end, size_t increment)
        : source_(std::move(source)),
          start_(start),
          end_(end),
          increment_(increment) {}

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

        if(index_ + increment_ < end_) {
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

private:
    bool first_ = true;
    StreamProviderPtr<T> source_;
    std::shared_ptr<T> current_;
    size_t index_ = 0;
    size_t start_;
    size_t end_;
    size_t increment_;


};

#endif
