#ifndef CONCATENATED_STREAM_PROVIDER_H
#define CONCATENATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include <list>

template<typename T, template<typename> class Pointer>
class ConcatenatedStreamProvider : public StreamProvider<T, Pointer> {

public:
    template<typename Iterator>
    ConcatenatedStreamProvider(Iterator begin, Iterator end)
        : source_(begin, end) {}

    ConcatenatedStreamProvider(StreamProviderPtr<T, Pointer> first,
                               StreamProviderPtr<T, Pointer> second) {
        source_.push_back(std::move(first));
        source_.push_back(std::move(second));
    }

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        while(!source_.empty()) {
            auto& provider = source_.front();
            if(provider->HasNext()) {
                next_ = provider->Next();
                return true;
            }
            source_.pop_front();
        }
        return false;
    }

private:
    std::list<StreamProviderPtr<T, Pointer>> source_;
    Pointer<T> next_;

};

#endif
