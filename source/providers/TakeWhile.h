#ifndef SCHEINERMAN_STREAM_PROVIDERS_TAKE_WHILE_H
#define SCHEINERMAN_STREAM_PROVIDERS_TAKE_WHILE_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Predicate>
class TakeWhile : public StreamProvider<T> {

public:
    TakeWhile(StreamProviderPtr<T> source, Predicate&& predicate)
        : source_(std::move(source)), predicate_(predicate) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(source_->advance()) {
            current_ = source_->get();
            if(!predicate_(*current_)) {
                current_.reset();
                return false;
            }
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "TakeWhile:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Predicate predicate_;
    std::shared_ptr<T> current_;
};

} /* namespace provider */
} /* namespace stream */

#endif
