#ifndef STREAM_BOOL_SPECIALIZATION_H
#define STREAM_BOOL_SPECIALIZATION_H

#include "SpecializationMacros.h"

#include <functional>

template<typename T>
class StreamImpl<T, Bool> : public virtual StreamImpl<T, Common> {

private:
    PRIVATE_USINGS;

public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;

    decltype(auto) filter()     { return Super::filter(to_bool); }
    decltype(auto) filter_not() { return Super::filter(not_(to_bool)); }
    decltype(auto) take_while() { return Super::take_while(to_bool); }
    decltype(auto) drop_while() { return Super::drop_while(to_bool); }

    decltype(auto) any()  { return Super::any(to_bool); }
    decltype(auto) all()  { return Super::all(to_bool); }
    decltype(auto) none() { return Super::none(to_bool); }

    decltype(auto) not_all() {
        this->check_vacant("not_all");
        return !Super::all(to_bool);
    }

    FRIENDS;

private:
    PRIVATE_CONSTRUCTORS;
};

#include "UndefSpecializationMacros.h"

#endif