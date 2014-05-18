#ifndef ADJACENT_DIFFERENCE_STREAM_PROVIDER_H
#define ADJACENT_DIFFERENCE_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Subtractor>
class AdjacentDifferenceStreamProvider
    : public StreamProvider<ReturnType<Subtractor, T&, T&>> {

public:
    using DiffType = ReturnType<Subtractor, T&, T&>;

    AdjacentDifferenceStreamProvider(StreamProviderPtr<T> source,
                                     Subtractor&& subtract)
        : source_(std::move(source)), subtract_(subtract) {}

    std::shared_ptr<DiffType> get() override {
        return result_;
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
            result_ = std::make_shared<DiffType>(subtract_(*second_, *first_));
            return true;
        }

        first_ = std::move(second_);
        if(source_->advance()) {
            second_ = source_->get();
            result_ = std::make_shared<DiffType>(subtract_(*second_, *first_));
            return true;
        }
        first_.reset();
        result_.reset();
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "AdjacentDifference:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<T> source_;
    Subtractor subtract_;
    std::shared_ptr<T> first_;
    std::shared_ptr<T> second_;
    std::shared_ptr<DiffType> result_;
    bool first_advance_ = true;
};

#endif
