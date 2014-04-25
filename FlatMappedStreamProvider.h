#ifndef FLAT_MAPPED_STREAM_PROVIDER_H
#define FLAT_MAPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Stream.h"
#include "Utility.h"

template<typename T, template<typename> class P> struct BasicStream;

template<typename T>
struct StreamIdentifier {
    using Type = void;
};

template<typename T, template<typename> class P>
struct StreamIdentifier<BasicStream<T, P>> { 
    using Type = T;
};

template<typename S> using StreamType = typename StreamIdentifier<S>::Type;

template<typename T>
struct IsStream {
    enum { value = false };
};

template<typename T, template<typename> class P>
struct IsStream<BasicStream<T, P>> { 
    enum { value = true };
};

template<typename T, template<typename> class Pointer,
         typename Transform, typename In>
class FlatMappedStreamProvider : public StreamProvider<T, Pointer> {

public:
    FlatMappedStreamProvider(StreamProviderPtr<In, Pointer> source,
                             Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(!first_ && current_stream_.source_->advance()) {
            current_ = current_stream_.source_->get();
            return true;
        }

        if(first_)
            first_ = false;

        while(source_->advance()) {
            current_stream_ = std::move(transform_(*source_->get()));
            if(current_stream_.source_->advance()) {
                current_ = current_stream_.source_->get();
                return true;
            }
        }

        return false;
    }

private:
    StreamProviderPtr<In, Pointer> source_;
    Transform transform_;
    BasicStream<T, Pointer> current_stream_;
    Pointer<T> current_;
    bool first_ = true;

};

#endif
