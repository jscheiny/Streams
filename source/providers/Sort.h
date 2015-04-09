#ifndef SCHEINERMAN_STREAM_PROVIDERS_SORT_H
#define SCHEINERMAN_STREAM_PROVIDERS_SORT_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <vector>
#include <queue>

namespace stream {
namespace provider {

template<typename T, typename RawCompare>
class Sort : public StreamProvider<T> {

public:
    Sort(StreamProviderPtr<T> source, RawCompare&& comparator)
        : source_(std::move(source)), sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.push(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty()) {
            current_.reset();
            return false;
        }
        current_ = sorted_.top();
        sorted_.pop();
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Sort:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    using PointerCompare = ComparePtrWrapper<T, RawCompare, true>;
    using PointerQueue = std::priority_queue<std::shared_ptr<T>,
                                             std::vector<std::shared_ptr<T>>,
                                             PointerCompare>;

    StreamProviderPtr<T> source_;
    PointerQueue sorted_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
