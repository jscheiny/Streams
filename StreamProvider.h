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



#endif
