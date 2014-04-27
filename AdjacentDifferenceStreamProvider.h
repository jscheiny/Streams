#ifndef ADJACENT_DIFFERENCE_STREAM_PROVIDER_H
#define ADJACENT_DIFFERENCE_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Subtract>
class AdjacentDifferenceStreamProvider
    : public StreamProvider<ReturnType<Subtract, T&, T&>> {

public:
    using DiffType = ReturnType<Subtract, T&, T&>;

    AdjacentDifferenceStreamProvider(StreamProviderPtr<T> source,
                                     Subtract&& subtract)
        : source_(std::move(source)), subtract_(subtract) {}

    std::shared_ptr<DiffType> get() override {
        return std::make_shared<DiffType>(subtract_(*second_, *first_));
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
                first_.reset();
                return false;
            }
            return true;
        }

        first_ = std::move(second_);
        if(source_->advance()) {
            second_ = source_->get();
            return true;
        }
        first_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Subtract subtract_;
    std::shared_ptr<T> first_;
    std::shared_ptr<T> second_;
    bool first_advance_ = true;
};

#endif
