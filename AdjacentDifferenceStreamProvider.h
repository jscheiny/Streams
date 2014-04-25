#ifndef ADJACENT_DIFFERENCE_STREAM_PROVIDER_H
#define ADJACENT_DIFFERENCE_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, template<typename> class Pointer, typename Subtract>
class AdjacentDifferenceStreamProvider 
    : public StreamProvider<ReturnType<Subtract, T&, T&>, Pointer> {

public:
    using DiffType = ReturnType<Subtract, T&, T&>;

    AdjacentDifferenceStreamProvider(StreamProviderPtr<T, Pointer> source,
                                     Subtract&& subtract)
        : source_(std::move(source)), subtract_(subtract) {}

    Pointer<DiffType> get() override {
        return move_unique(subtract_(*second_, *first_));
    }

    bool advance() override {
        if(first_advance_) {
            first_advance_ = false;
            if(source_->advance()) {
                first_ = source_->get();
            } else {
                return false;
            }
            if(source_->advance()) {
                second_ = source_->get();
            } else {
                return false;
            }
            return true;
        }

        first_ = std::move(second_);
        if(source_->advance()) {
            second_ = source_->get();
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Subtract subtract_;
    Pointer<T> first_;
    Pointer<T> second_;
    bool first_advance_ = true;
};

#endif