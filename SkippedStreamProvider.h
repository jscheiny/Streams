#ifndef SKIPPED_STREAM_PROVIDER_H
#define SKIPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer>
class SkippedStreamProvider : public StreamProvider<T, Pointer> {

public:
    SkippedStreamProvider(StreamProviderPtr<T, Pointer> source, size_t skip)
        : source_(std::move(source)), skip_(skip) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(skip_ != 0) {
            for(; skip_ != 0; skip_--) {
                if(source_->advance()) {
                    current_ = source_->get();
                } else {
                    return false;
                }
            }
            return true;
        }

        if(source_->advance()) {
            current_ = source_->get();
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Pointer<T> current_;
    std::size_t skip_;

};

#endif
