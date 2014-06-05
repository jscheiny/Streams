#ifndef PROVIDER_H
#define PROVIDER_H

#include "../StreamError.h"

#include <memory>

namespace stream {
namespace provider {

template<typename T>
struct StreamProvider {

public:
    struct Iterator;

    virtual std::shared_ptr<T> get() = 0;

    bool advance() {
        try {
            return advance_impl();
        } catch(stream::StopStream& stop) {
            return false;
        } catch(...) {
            throw;
        }
    }

    Iterator begin();
    Iterator end();

    virtual std::pair<int, int> print(std::ostream& os, int indent) const = 0;

protected:
    virtual bool advance_impl() = 0;

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

} /* namespace provider */
} /* namespace stream */

#include "StreamProviderIterator.h"

template<typename T>
typename stream::provider::StreamProvider<T>::Iterator
stream::provider::StreamProvider<T>::begin() {
    return Iterator::begin(this);
}

template<typename T>
typename stream::provider::StreamProvider<T>::Iterator
stream::provider::StreamProvider<T>::end() {
    return Iterator::end(this);
}

namespace stream {

template<typename T>
using StreamProviderPtr = std::unique_ptr<provider::StreamProvider<T>>;

template<template<typename...> class Provider,
         typename T,
         typename... TemplateArgs,
         typename... ConstructorArgs>
StreamProviderPtr<T> make_stream_provider(ConstructorArgs&&... args) {
    return StreamProviderPtr<T>(
        new Provider<T, TemplateArgs...>(
            std::forward<ConstructorArgs>(args)...));
}

} /* namespace stream */

#endif
