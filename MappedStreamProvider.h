#ifndef MAPPED_STREAM_PROVIDER_H
#define MAPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, template<typename> class Pointer, typename Transform>
class MappedStreamProvider : public StreamProvider<T, Pointer> {

public:
    MappedStreamProvider(StreamProviderPtr<T, Pointer> source, Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(source_->advance()) {
            auto preimage = source_->get();
            current_ = move_unique(transform_(*preimage));
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Transform transform_;
    Pointer<T> current_;

};

#endif
