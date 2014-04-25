#ifndef TAKE_WHILE_STREAM_PROVIDER_H
#define TAKE_WHILE_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer, typename Predicate>
class TakeWhileStreamProvider : public StreamProvider<T, Pointer> {

public:
    TakeWhileStreamProvider(StreamProviderPtr<T, Pointer> source,
                            Predicate&& predicate, bool end)
        : source_(std::move(source)),
          predicate_(predicate),
          end_condition_(end) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(source_->advance()) {
            current_ = source_->get();
            if(predicate_(*current_) == end_condition_) {
                return false;
            }
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Predicate predicate_;
    bool end_condition_;
    Pointer<T> current_;
};


#endif
