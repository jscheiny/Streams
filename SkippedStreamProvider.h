#ifndef SKIPPED_STREAM_PROVIDER_H
#define SKIPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer>
class SkippedStreamProvider : public StreamProvider<T, Pointer> {

public:
    SkippedStreamProvider(StreamProviderPtr<T, Pointer> source, size_t skip)
        : source_(std::move(source)), skip_(skip) {}

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        if(skip_ != 0) {
            for(; skip_ != 0; skip_--) {
                if(source_->HasNext()) {
                    next_ = source_->Next();
                } else {
                    return false;
                }
            }
            return true;
        }

        if(source_->HasNext()) {
            next_ = source_->Next();
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Pointer<T> next_;
    std::size_t skip_;

};

#endif
