#ifndef FILTERED_STREAM_PROVIDER_H
#define FILTERED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer, typename Predicate>
class FilteredStreamProvider : public StreamProvider<T, Pointer> {

public:
    FilteredStreamProvider(StreamProviderPtr<T, Pointer> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        while(source_->advance()) {
            current_ = source_->get();
            if(predicate_(*current_)) {
                return true;
            }
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Predicate predicate_;
    Pointer<T> current_;
};


#endif
