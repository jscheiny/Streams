#ifndef TAKE_WHILE_STREAM_PROVIDER_H
#define TAKE_WHILE_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, typename Predicate>
class TakeWhileStreamProvider : public StreamProvider<T> {

public:
    TakeWhileStreamProvider(StreamProviderPtr<T> source, Predicate&& predicate,
                            bool end)
        : source_(std::move(source)),
          predicate_(predicate),
          end_condition_(end) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(source_->advance()) {
            current_ = source_->get();
            if(predicate_(*current_) == end_condition_) {
                current_.reset();
                return false;
            }
            return true;
        }
        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Predicate predicate_;
    bool end_condition_;
    std::shared_ptr<T> current_;
};


#endif
