#ifndef PROVIDER_H
#define PROVIDER_H

#include "StreamError.h"

#include <memory>

template<typename T>
struct StreamProvider {
public:
    virtual std::shared_ptr<T> get() = 0;
    virtual bool advance() = 0;

    virtual std::pair<int, int> print(std::ostream& os, int indent) const = 0;

    class Iterator;

    Iterator begin();
    Iterator end();

protected:
    static void print_indent_arrow(std::ostream& os, int indent) {
        for(int i = 0; i < indent - 1; i++) {
            os << "  ";
        }
        if(indent != 0) {
            os << "> ";
        }
    }


};

#include "StreamProviderIteratorImpl.h"

template<typename T>
typename StreamProvider<T>::Iterator StreamProvider<T>::begin() {
    return Iterator::begin(this);
}

template<typename T>
typename StreamProvider<T>::Iterator StreamProvider<T>::end() {
    return Iterator::end(this);
}

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
