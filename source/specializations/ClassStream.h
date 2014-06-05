#ifndef SPECIALIZATIONS_CLASS_STREAM_H
#define SPECIALIZATIONS_CLASS_STREAM_H

#include <functional>

#include "SpecializationMacros.h"
#define OPERATOR_OVERRIDE(method) \
    template<typename F> \
    decltype(auto) method (F&& f) \
        { return Super :: method (std::forward<F>(f)); } \
    template<typename R> \
    decltype(auto) method (R (Type::*member)()) \
        { return Super :: method (std::mem_fn(member)); } \
    template<typename R> \
    decltype(auto) method (R (Type::*member)() const) \
        { return Super :: method (std::mem_fn(member)); }

template<typename T>
class StreamImpl<T, Class> : public virtual StreamImpl<T, Common> {

private:
    PRIVATE_USINGS;
    using Type = std::remove_pointer_t<T>;

public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
    MOVE_SEMANTICS(Class);

    OPERATOR_OVERRIDE(filter);
    OPERATOR_OVERRIDE(take_while);
    OPERATOR_OVERRIDE(drop_while);
    OPERATOR_OVERRIDE(peek);
    OPERATOR_OVERRIDE(map);
    OPERATOR_OVERRIDE(flat_map);

    OPERATOR_OVERRIDE(any);
    OPERATOR_OVERRIDE(all);
    OPERATOR_OVERRIDE(none);
    OPERATOR_OVERRIDE(for_each);

    FRIENDS;

private:
    PRIVATE_CONSTRUCTORS;
};

#undef OPERATOR_OVERRIDE
#include "UndefSpecializationMacros.h"

#endif