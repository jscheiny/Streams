#ifndef PROVIDER_H
#define PROVIDER_H

#include <memory>

template<typename T>
struct StreamProvider {
public:
    virtual std::shared_ptr<T> get() = 0;
    virtual bool advance() = 0;

};

template<typename T>
using StreamProviderPtr = std::unique_ptr<StreamProvider<T>>;

template<template<typename...> class Provider,
         typename T,
         typename... TemplateArgs,
         typename... ConstructorArgs>
StreamProviderPtr<T> make_stream_provider(ConstructorArgs&&... args) {
    return StreamProviderPtr<T>(
        new Provider<T, TemplateArgs...>(
            std::forward<ConstructorArgs>(args)...));
}

#endif
