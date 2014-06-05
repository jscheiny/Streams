#ifndef SPECIALIZATIONS_NUMBER_STREAM_H
#define SPECIALIZATIONS_NUMBER_STREAM_H

#include <functional>

#include "SpecializationMacros.h"

namespace stream {

template<typename T>
class StreamImpl<T, Number> : public virtual StreamImpl<T, Common> {

private:
    PRIVATE_USINGS;
public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
    MOVE_SEMANTICS(Number);
    FRIENDS;

    T sum() {
        return Super::sum(0);
    }

    T product() {
        return Super::product(1);
    }

private:
    PRIVATE_CONSTRUCTORS;

};

} /* namespace stream */

#include "UndefSpecializationMacros.h"

#endif