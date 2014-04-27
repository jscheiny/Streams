#ifndef ITERATED_STREAM_PROVIDER_H
#define ITERATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

#include <iostream>
#include <type_traits>

template<typename T, typename Function>
class IteratedStreamProvider : public StreamProvider<T> {

public:
    IteratedStreamProvider(T initial, Function&& function)
        : function_(function),
          current_(std::make_shared<T>(initial)) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        current_ = std::make_shared<T>(function_(*current_));
        return true;
    }

private:
    Function function_;
    std::shared_ptr<T> current_;
};


#endif
