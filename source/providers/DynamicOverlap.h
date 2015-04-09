#ifndef SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_OVERLAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_DYNAMIC_OVERLAP_H

#include "StreamProvider.h"

#include <deque>

namespace stream {
namespace provider {

template<typename T>
class DynamicOverlap : public StreamProvider<std::deque<T>> {

private:
    using Result = std::deque<T>;

public:
    DynamicOverlap(StreamProviderPtr<T> source, size_t N) : source_(std::move(source)), N_(N) {}

    std::shared_ptr<Result> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            current_ = std::make_shared<Result>();
            for(size_t i = 0; i < N_; i++) {
                if(source_->advance()) {
                    current_->emplace_back(std::move(*source_->get()));
                } else {
                    current_.reset();
                    return false;
                }
            }
            return true;
        }

        if(source_->advance()) {
            current_ = std::make_shared<Result>(*current_);
            current_->pop_front();
            current_->emplace_back(std::move(*source_->get()));
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Overlapped[" << N_ << "]:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    bool first_ = true;
    StreamProviderPtr<T> source_;
    std::shared_ptr<Result> current_;
    const size_t N_;

};

} /* namespace provider */
} /* namespace stream */

#endif

