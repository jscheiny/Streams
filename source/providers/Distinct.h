#ifndef SCHEINERMAN_STREAM_PROVIDERS_DISTINCT_H
#define SCHEINERMAN_STREAM_PROVIDERS_DISTINCT_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <set>

namespace stream {
namespace provider {

template<typename T, typename RawCompare>
class Distinct : public StreamProvider<T> {

public:
    Distinct(StreamProviderPtr<T> source, RawCompare&& comparator)
        : source_(std::move(source)), sorted_(PointerCompare(std::forward<RawCompare>(comparator))) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            while(source_->advance()) {
                sorted_.insert(source_->get());
            }
            first_ = false;
        }
        if(sorted_.empty()) {
            current_.reset();
            return false;
        }
        auto itr = sorted_.begin();
        current_ = *itr;
        sorted_.erase(itr);
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Distinct:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    using PointerCompare = ComparePtrWrapper<T, RawCompare>;
    using PointerQueue = std::set<std::shared_ptr<T>, PointerCompare>;

    StreamProviderPtr<T> source_;
    PointerQueue sorted_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
