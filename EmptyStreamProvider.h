#ifndef EMPTY_STREAM_PROVIDER_H
#define EMPTY_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, template<typename> class Pointer>
class EmptyStreamProvider : public StreamProvider<T, Pointer> {

public:
    Pointer<T> get() override {
        return nullptr;
    }

    bool advance() override {
        return false;
    }

};

#endif
