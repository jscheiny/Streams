#ifndef DISTINCT_STREAM_PROVIDER_H
#define DISTINCT_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

#include <set>

template<typename T, typename RawCompare>
class DistinctStreamProvider : public StreamProvider<T> {

public:
    DistinctStreamProvider(StreamProviderPtr<T> source,
                           RawCompare&& comparator)
        : source_(std::move(source)),
          sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.insert(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty()) {
            current_.reset();
            return false;
        }
        auto itr = sorted_.begin();
        current_ = *itr;
        sorted_.erase(itr);
        return true;
    }

private:
    using PointerCompare = ComparePtrWrapper<T, RawCompare>;
    using PointerQueue = std::set<std::shared_ptr<T>, PointerCompare>;

    StreamProviderPtr<T> source_;
    PointerQueue sorted_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

#endif
