#ifndef ADJACENT_DISTINCT_STREAM_PROVIDER_H
#define ADJACENT_DISTINCT_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Equal>
class AdjacentDistinctStreamProvider : public StreamProvider<T> {

public:
    AdjacentDistinctStreamProvider(StreamProviderPtr<T> source, Equal&& equal)
        : source_(std::move(source)), equal_(std::forward<Equal>(equal_)) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(first_) {
            first_ = false;
            if(source_->advance()) {
                current_ = source_->get();
                return true;
            }
            return false;
        }

        while(source_->advance()) {
            auto next = source_->get();
            if(!equal_(*current_, *next)) {
                current_ = next;
                return true;
            }
        }

        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Equal equal_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

#endif
