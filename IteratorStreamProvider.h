#ifndef ITERATOR_STREAM_PROVIDER_H
#define ITERATOR_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, typename Iterator>
class IteratorStreamProvider : public StreamProvider<T> {

public:
    IteratorStreamProvider(Iterator begin, Iterator end)
        : current_(begin), end_(end) {}

    std::shared_ptr<T> get() override {
        return std::make_shared<T>(std::move(*current_));
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
