#ifndef DISTINCT_STREAM_PROVIDER_H
#define DISTINCT_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

#include <set>

template<typename T, template<typename> class Pointer, typename RawCompare>
class DistinctStreamProvider : public StreamProvider<T, Pointer> {

public:
    DistinctStreamProvider(StreamProviderPtr<T, Pointer> source,
                           RawCompare&& comparator)
        : source_(std::move(source)),
          sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.insert(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty())
            return false;
        // Based on some research, this is what is widely agreed as the best way
        // to get a move only type out of the end of a set.
        auto itr = sorted_.begin();
        current_ = std::move(const_cast<Pointer<T>&>(*itr));
        sorted_.erase(itr);
        return true;
    }

private:
    using PointerCompare = ComparePtrWrapper<T, Pointer, RawCompare>;
    using PointerQueue = std::set<Pointer<T>, PointerCompare>;

    StreamProviderPtr<T, Pointer> source_;
    PointerQueue sorted_;
    Pointer<T> current_;
    bool first_ = true;
};

#endif
