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
    MOVE_SEMANTICS(Bool | Number);
    FRIENDS;
private:
    PRIVATE_CONSTRUCTORS;

};

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
    MOVE_SEMANTICS(Bool | Class);
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
    static_assert(Tags != (Class | Number),
        "Illegal Stream specialization for classes and numbers. "
        "This is considered impossible, please report this.");
    static_assert(Tags != (Class | Number | Bool),
        "Illegal Stream specialization for classes, numbers, and bools. "
        "This is considered impossible, please report this.");
    static_assert(Tags > 0 && Tags < (Class | Bool | Number),
        "Illegal Stream with no specialization. "
        "This is considered impossible, please report this.");
};
#include "UndefSpecializationMacros.h"

#endif