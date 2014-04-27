#ifndef SORTED_STREAM_PROVIDER_H
#define SORTED_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

#include <vector>
#include <queue>

template<typename T, typename RawCompare>
class SortedStreamProvider : public StreamProvider<T> {

public:
    SortedStreamProvider(StreamProviderPtr<T> source,
                         RawCompare&& comparator)
        : source_(std::move(source)),
          sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.push(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty()) {
            current_.reset();
            return false;
        }
        current_ = sorted_.top();
        sorted_.pop();
        return true;
    }

private:
    using PointerCompare = ComparePtrWrapper<T, RawCompare, true>;
    using PointerQueue = std::priority_queue<std::shared_ptr<T>,
                                             std::vector<std::shared_ptr<T>>,
                                             PointerCompare>;

    StreamProviderPtr<T> source_;
    PointerQueue sorted_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

#endif
