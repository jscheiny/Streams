#ifndef REPEATED_STREAM_PROVIDER_H
#define REPEATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T>
class RepeatedStreamProvider : public StreamProvider<T> {

public:
    RepeatedStreamProvider(const T& value)
        : value_(std::make_shared<T>(value)) {}

    RepeatedStreamProvider(T&& value)
        : value_(std::make_shared<T>(value)) {}

    std::shared_ptr<T> get() override {
        return value_;
    }

    bool advance() override {
        return true;
    }

private:
    std::shared_ptr<T> value_;

};


#endif
