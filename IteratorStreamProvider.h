#ifndef ITERATOR_STREAM_PROVIDER_H
#define ITERATOR_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer, typename Iterator>
class IteratorStreamProvider : public StreamProvider<T, Pointer> {

public:
    IteratorStreamProvider(Iterator begin, Iterator end)
        : current_(begin), end_(end) {}

    Pointer<T> Next() override {
        auto value = std::make_unique<T>(std::move(*current_));
        current_++;
        return value;
    }

    bool HasNext() override {
        return current_ != end_;
    }

private:
    Iterator current_;
    Iterator end_;

};

#endif
