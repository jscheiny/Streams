#ifndef FILTERED_STREAM_PROVIDER_H
#define FILTERED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, typename Predicate>
class FilteredStreamProvider : public StreamProvider<T> {

public:
    FilteredStreamProvider(StreamProviderPtr<T> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        while(source_->advance()) {
            current_ = source_->get();
            if(predicate_(*current_)) {
                return true;
            }
        }
        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Predicate predicate_;
    std::shared_ptr<T> current_;
};


#endif
