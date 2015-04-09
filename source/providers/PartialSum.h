#ifndef SCHEINERMAN_STREAM_PROVIDERS_PARTIAL_SUM_H
#define SCHEINERMAN_STREAM_PROVIDERS_PARTIAL_SUM_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Adder>
class PartialSum : public StreamProvider<T> {

public:
    PartialSum(StreamProviderPtr<T> source, Adder&& add)
        : source_(std::move(source)), add_(add) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            if(source_->advance()) {
                current_ = source_->get();
                return true;
            }
            return false;
        }

        if(source_->advance()) {
            current_ = std::make_shared<T>(add_(
                std::move(*current_),
                std::move(*source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "PartialSum:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Adder add_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
