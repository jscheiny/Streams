#ifndef SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DIFFERENCE_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <type_traits>

namespace stream {
namespace provider {

template<typename T, typename Subtractor>
class AdjacentDifference : public StreamProvider<std::result_of_t<Subtractor(T&, T&)>> {

public:
    using DiffType = std::result_of_t<Subtractor(T&, T&)>;

    AdjacentDifference(StreamProviderPtr<T> source, Subtractor&& subtract)
        : source_(std::move(source)), subtract_(subtract) {}

    std::shared_ptr<DiffType> get() override {
        return result_;
    }

    bool advance_impl() override {
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

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "AdjacentDifference:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Subtractor subtract_;
    std::shared_ptr<T> first_;
    std::shared_ptr<T> second_;
    std::shared_ptr<DiffType> result_;
    bool first_advance_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
