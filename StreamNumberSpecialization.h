#ifndef STREAM_NUMBER_SPECIALIZATION_H
#define STREAM_NUMBER_SPECIALIZATION_H

#include "SpecializationMacros.h"

#include <functional>

template<typename T>
class StreamImpl<T, Number> : public virtual StreamImpl<T, Common> {

private:
    PRIVATE_USINGS;
public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
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

#include "UndefSpecializationMacros.h"

#endif