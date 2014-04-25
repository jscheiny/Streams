#ifndef ITERATOR_STREAM_PROVIDER_H
#define ITERATOR_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, template<typename> class Pointer, typename Iterator>
class IteratorStreamProvider : public StreamProvider<T, Pointer> {

public:
    IteratorStreamProvider(Iterator begin, Iterator end)
        : current_(begin), end_(end) {}

    Pointer<T> get() override {
        return make_unique<T>(std::move(*current_));
    }

    bool advance() override {
        if(first_) {
            first_ = false;
            return current_ != end_;
        }
        ++current_;
        return current_ != end_;
    }

private:
    bool first_ = true;
    Iterator current_;
    Iterator end_;

};

#endif
