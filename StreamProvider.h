#ifndef PROVIDER_H
#define PROVIDER_H

#include <memory>

template<typename T, template<typename> class Pointer = std::unique_ptr>
struct StreamProvider {
public:
    virtual Pointer<T> get() = 0;
    virtual bool advance() = 0;

};

template<typename T, template<typename> class P>
using StreamProviderPtr = std::unique_ptr<StreamProvider<T, P>>;

template<template<typename, template<typename> class, typename...> class Provider,
         typename T, template<typename> class Pointer,
         typename... TemplateArgs,
         typename... ConstructorArgs>
StreamProviderPtr<T, Pointer> make_stream_provider(ConstructorArgs&&... args) {
    return StreamProviderPtr<T, Pointer>(
        new Provider<T, Pointer, TemplateArgs...>(
            std::forward<ConstructorArgs>(args)...));
}

#endif
