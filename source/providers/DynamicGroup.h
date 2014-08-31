#ifndef PROVIDERS_DYNAMIC_GROUP_H
#define PROVIDERS_DYNAMIC_GROUP_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <vector>

namespace stream {
namespace provider {

template<typename T>
class DynamicGroup : public StreamProvider<std::vector<T>> {

public:
    DynamicGroup(StreamProviderPtr<T> source, size_t N) : source_(std::move(source)), N_(N) {}

    std::shared_ptr<std::vector<T>> get() override {
        return current_;
    }

    bool advance_impl() override {
        current_ = std::make_shared<std::vector<T>>();
        current_->reserve(N_);
        for(int i = 0; i < N_; i++) {
            if(source_->advance()) {
                current_->emplace_back(std::move(*source_->get()));
            } else {
                current_.reset();
                return false;
            }
        }
        return true;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "Grouped[" << N_ << "]:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<T> source_;
    std::shared_ptr<std::vector<T>> current_;
    const size_t N_;

};

} /* namespace provider */
} /* namespace stream */

#endif
