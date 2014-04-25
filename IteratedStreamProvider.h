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
          current_(std::make_unique<T>(initial)),
          current_copy_(*current_) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        current_ = move_unique(function_(current_copy_));
        current_copy_ = *current_;
        return true;
    }

private:
    Function function_;
    Pointer<T> current_;
    typename std::decay<T>::type current_copy_;
};


#endif
