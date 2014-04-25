#ifndef DROP_WHILE_STREAM_PROVIDER_H
#define DROP_WHILE_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer, typename Predicate>
class DropWhileStreamProvider : public StreamProvider<T, Pointer> {

public:
    DropWhileStreamProvider(StreamProviderPtr<T, Pointer> source,
                           Predicate&& predicate, bool end)
        : source_(std::move(source)),
          predicate_(predicate),
          end_condition_(end) {}

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        if(!dropped_) {
            while(source_->HasNext()) {
                next_ = source_->Next();
                if(predicate_(*next_) == end_condition_) {
                    dropped_ = true;
                    return true;
                }
            }
            dropped_ = true;
            return false;
        }
        if(source_->HasNext()) {
            next_ = source_->Next();
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Predicate predicate_;
    bool end_condition_;
    Pointer<T> next_;
    bool dropped_ = false;
};


#endif
