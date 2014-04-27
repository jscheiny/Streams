#ifndef SKIPPED_STREAM_PROVIDER_H
#define SKIPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class SkippedStreamProvider : public StreamProvider<T> {

public:
    SkippedStreamProvider(StreamProviderPtr<T> source, size_t skip)
        : source_(std::move(source)), skip_(skip) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(skip_ != 0) {
            for(; skip_ != 0; skip_--) {
                if(source_->advance()) {
                    current_ = source_->get();
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if(source_->advance()) {
            current_ = source_->get();
            return true;
        }
        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    std::shared_ptr<T> current_;
    std::size_t skip_;

};

#endif
