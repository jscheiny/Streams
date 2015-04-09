#ifndef SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DISTINCT_H
#define SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DISTINCT_H

#include "StreamProvider.h"
#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Equal>
class AdjacentDistinct : public StreamProvider<T> {

public:
    AdjacentDistinct(StreamProviderPtr<T> source, Equal&& equal)
        : source_(std::move(source)), equal_(std::forward<Equal>(equal_)) {}

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

        while(source_->advance()) {
            auto next = source_->get();
            if(!equal_(*current_, *next)) {
                current_ = next;
                return true;
            }
        }

        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "AdjacentDistinct:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Equal equal_;
    std::shared_ptr<T> current_;
    bool first_ = true;
};

} /* namespace provider */
} /* namespace stream */

#endif
