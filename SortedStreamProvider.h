#ifndef SORTED_STREAM_PROVIDER_H
#define SORTED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include <vector>
#include <queue>

/*
 * There's something fundamentally annoying about the design here. If the pointer type
 * is unique_ptr, then we have to do an incredibly ugly const_cast to get the pointer
 * out of the priority queue via a move operation. The other possibility is to use a
 * vector, read it in and then sort it. However, this is not as efficient.
 * What to do?
 */

template<typename T, template<typename> class Pointer, typename Compare>
class ComparePtrWrapper {
public:
    ComparePtrWrapper(Compare&& comparator) : comparator_(comparator) {}

    bool operator() (const Pointer<T>& left, const Pointer<T>& right) {
        return comparator_(*right, *left);
    }

private:
    Compare comparator_;
};

template<typename T, template<typename> class Pointer, typename RawCompare>
class SortedStreamProvider : public StreamProvider<T, Pointer> {

public:
    SortedStreamProvider(StreamProviderPtr<T, Pointer> source,
                             RawCompare&& comparator)
        : source_(std::move(source)),
          sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.push(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty())
            return false;
        current_ = std::move(const_cast<Pointer<T>&>(sorted_.top())); // Ugh.
        sorted_.pop();
        return true;
    }

private:
    using PointerCompare = ComparePtrWrapper<T, Pointer, RawCompare>;
    using PointerQueue = std::priority_queue<Pointer<T>, std::vector<Pointer<T>>, PointerCompare>;

    StreamProviderPtr<T, Pointer> source_;
    PointerQueue sorted_;
    Pointer<T> current_;
    bool first_ = true;
};

#endif
