#ifndef FILTERED_STREAM_PROVIDER_H
#define FILTERED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer, typename Predicate>
class FilteredStreamProvider : public StreamProvider<T, Pointer> {

public:
    FilteredStreamProvider(StreamProviderPtr<T, Pointer> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        while(source_->HasNext()) {
            next_ = source_->Next();
            if(predicate_(*next_)) {
                return true;
            }
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Predicate predicate_;
    Pointer<T> next_;
};


#endif
