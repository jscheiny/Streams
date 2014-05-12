#ifndef DROP_WHILE_STREAM_PROVIDER_H
#define DROP_WHILE_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, typename Predicate>
class DropWhileStreamProvider : public StreamProvider<T> {

public:
    DropWhileStreamProvider(StreamProviderPtr<T> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(!dropped_) {
            dropped_ = true;
            while(source_->advance()) {
                current_ = source_->get();
                if(!predicate_(*current_)) {
                    return true;
                }
            }
            current_.reset();
            return false;
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
    Predicate predicate_;
    std::shared_ptr<T> current_;
    bool dropped_ = false;
};

#endif
