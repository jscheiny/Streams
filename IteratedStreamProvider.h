#ifndef ITERATED_STREAM_PROVIDER_H
#define ITERATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

#include <iostream>
#include <type_traits>

template<typename T, template<typename> class Pointer, typename Function>
class IteratedStreamProvider : public StreamProvider<T, Pointer> {

public:
    IteratedStreamProvider(T initial, Function&& function)
        : function_(function),
          next_(std::make_unique<T>(initial)),
          next_copy_(*next_) {}

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        next_ = move_unique(function_(next_copy_));
        next_copy_ = *next_;
        return true;
    }

private:
    Function function_;
    Pointer<T> next_;
    typename std::decay<T>::type next_copy_;
};


#endif
