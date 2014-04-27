#ifndef PARTIAL_SUM_STREAM_PROVIDER_H
#define PARTIAL_SUM_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T, typename Adder>
class PartialSumStreamProvider : public StreamProvider<T> {

public:
    PartialSumStreamProvider(StreamProviderPtr<T> source, Adder&& add)
        : source_(std::move(source)), add_(add) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(first_) {
            first_ = false;
            if(source_->advance()) {
                current_ = source_->get();
                return true;
            }
            return false;
        }

        if(source_->advance()) {
            current_ = std::make_shared<T>(add_(*current_, *source_->get()));
            return true;
        }
        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Adder add_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

#endif