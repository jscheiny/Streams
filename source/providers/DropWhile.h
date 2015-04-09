#ifndef SCHEINERMAN_STREAM_PROVIDERS_DROP_WHILE_H
#define SCHEINERMAN_STREAM_PROVIDERS_DROP_WHILE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Predicate>
class DropWhile : public StreamProvider<T> {

public:
    DropWhile(StreamProviderPtr<T> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(!dropped_) {
            dropped_ = true;
            while(source_->advance()) {
                current_ = source_->get();
                if(!predicate_(*current_)) {
                    return true;
                }
            }
            current_.reset();
            return false;
        }
        if(source_->advance()) {
            current_ = source_->get();
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "DropWhile:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Predicate predicate_;
    std::shared_ptr<T> current_;
    bool dropped_ = false;
};

} /* namespace provider */
} /* namespace stream */

#endif
