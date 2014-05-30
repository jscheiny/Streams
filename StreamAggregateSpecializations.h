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

template<typename T, int Tags>
class StreamImpl {
    static_assert(Tags != (Class | Pointer),
        "Illegal Stream specialization for classes and pointers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Pointer | Number),
        "Illegal Stream specialization for pointers and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Class | Pointer | Bool),
        "Illegal Stream specialization for classes, pointers, and bools. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Class | Pointer | Number),
        "Illegal Stream specialization for classes, pointers, and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Class | Bool | Number),
        "Illegal Stream specialization for classes, bools, and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Pointer | Bool | Number),
        "Illegal Stream specialization for pointers, bools, and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Class | Pointer | Bool | Number),
        "Illegal Stream specialization for classes, pointers, bools, and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags > 0 && Tags < (Class | Pointer | Bool | Number),
        "Illegal Stream with no specialization. "
        "This is considered impossible, please report this.");
};
#include "UndefSpecializationMacros.h"

#endif