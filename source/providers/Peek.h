#ifndef SCHEINERMAN_STREAM_PROVIDERS_PEEK_H
#define SCHEINERMAN_STREAM_PROVIDERS_PEEK_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Action>
class Peek : public StreamProvider<T> {

public:
    Peek(StreamProviderPtr<T> source, Action&& action)
        : source_(std::move(source)), action_(action) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(source_->advance()) {
            current_ = source_->get();
            action_(*current_);
            return true;
        }
        current_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Peek:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<T> source_;
    Action action_;
    std::shared_ptr<T> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
