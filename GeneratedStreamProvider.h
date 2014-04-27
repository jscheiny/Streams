#ifndef GENERATED_STREAM_PROVIDER
#define GENERATED_STREAM_PROVIDER

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, typename Generator>
class GeneratedStreamProvider : public StreamProvider<T> {

public:
    GeneratedStreamProvider(Generator&& generator)
        : generator_(generator) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        current_ = std::make_shared<T>(generator_());
        return true;
    }

private:
    Generator generator_;
    std::shared_ptr<T> current_;

};


#endif
