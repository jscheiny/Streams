#ifndef GENERATED_STREAM_PROVIDER
#define GENERATED_STREAM_PROVIDER

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, template<typename> class Pointer, typename Generator>
class GeneratedStreamProvider : public StreamProvider<T, Pointer> {

public:
    GeneratedStreamProvider(Generator&& generator)
        : generator_(generator) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        current_ = move_unique(generator_());
        return true;
    }

private:
    Generator generator_;
    Pointer<T> current_;

};


#endif
