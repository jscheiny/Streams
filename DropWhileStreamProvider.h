#ifndef DROP_WHILE_STREAM_PROVIDER_H
#define DROP_WHILE_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, typename Predicate>
class DropWhileStreamProvider : public StreamProvider<T> {

public:
    DropWhileStreamProvider(StreamProviderPtr<T> source,
                           Predicate&& predicate, bool end)
        : source_(std::move(source)), predicate_(predicate), end_(end) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(!dropped_) {
            dropped_ = true;
            while(source_->advance()) {
                current_ = source_->get();
                if(predicate_(*current_) == end_) {
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
    bool end_;
    std::shared_ptr<T> current_;
    bool dropped_ = false;
};

#endif
