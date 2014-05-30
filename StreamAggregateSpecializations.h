#ifndef STREAM_AGGREGATE_SPECIALIZATIONS_H
#define STREAM_AGGREGATE_SPECIALIZATIONS_H

#include "SpecializationMacros.h"

template<typename T>
class StreamImpl<T, Bool | Number> 
    : public StreamImpl<T, Bool>, public StreamImpl<T, Number> {

private:
    PRIVATE_USINGS;
public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
    FRIENDS;
private:
    PRIVATE_CONSTRUCTORS;

};

#define OPERATOR_OVERRIDE(method) \
    decltype(auto) method () \
        { return StreamImpl<T, Bool> :: method (); } \
    template<typename F> \
    decltype(auto) method (F&& f) \
        { return StreamImpl<T, Pointer> :: method (std::forward<F>(f)); }

template<typename T>
class StreamImpl<T, Bool | Pointer>
    : public StreamImpl<T, Bool>, public StreamImpl<T, Pointer> {

private:
    PRIVATE_USINGS;
    using Type = std::remove_pointer_t<T>;

public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
    FRIENDS;

    OPERATOR_OVERRIDE(filter);
    OPERATOR_OVERRIDE(take_while);
    OPERATOR_OVERRIDE(drop_while);

private:
    PRIVATE_CONSTRUCTORS;

};

#undef OPERATOR_OVERRIDE

#define OPERATOR_OVERRIDE(method) \
    decltype(auto) method () \
        { return StreamImpl<T, Bool> :: method (); } \
    template<typename F> \
    decltype(auto) method (F&& f) \
        { return StreamImpl<T, Class> :: method (std::forward<F>(f)); }

template<typename T>
class StreamImpl<T, Bool | Class>
    : public StreamImpl<T, Bool>, public StreamImpl<T, Class> {

private:
    PRIVATE_USINGS;

public:
    PUBLIC_USINGS;
    PUBLIC_CONSTRUCTORS;
    FRIENDS;

    OPERATOR_OVERRIDE(filter);
    OPERATOR_OVERRIDE(take_while);
    OPERATOR_OVERRIDE(drop_while);

private:
    PRIVATE_CONSTRUCTORS;

};

#undef OPERATOR_OVERRIDE

#include "UndefSpecializationMacros.h"

#endif