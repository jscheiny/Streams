#ifndef PROVIDER_H
#define PROVIDER_H

#include <memory>

template<typename T>
std::unique_ptr<T> move_unique(T&& t) {
    return std::make_unique<T>(t);
}

template<typename T, template<typename> class Pointer = std::unique_ptr>
struct StreamProvider {
    virtual Pointer<T> Next() = 0;
    virtual bool HasNext() = 0;
};

template<typename T, template<typename> class P>
using StreamProviderPtr = std::unique_ptr<StreamProvider<T, P>>;

#endif
