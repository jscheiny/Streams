#ifndef EMPTY_STREAM_PROVIDER_H
#define EMPTY_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class EmptyStreamProvider : public StreamProvider<T> {

public:
    std::shared_ptr<T> get() override {
        return nullptr;
    }

    bool advance() override {
        return false;
    }

};

#endif
