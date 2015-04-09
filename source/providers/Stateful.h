#ifndef SCHEINERMAN_STREAM_PROVIDERS_STATEFUL_H
#define SCHEINERMAN_STREAM_PROVIDERS_STATEFUL_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <list>

namespace stream {
namespace provider {

template<typename T>
class Stateful : public StreamProvider<T> {

public:
    Stateful(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

    std::shared_ptr<T> get() override {
        return *current_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            while(source_->advance()) {
                state_.push_back(source_->get());
            }
            current_ = state_.begin();
            return current_ != state_.end();
        }
        ++current_;
        return current_ != state_.end();
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "StatePoint:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    using List = std::list<std::shared_ptr<T>>;
    using Iterator = typename List::iterator;

    bool first_ = true;
    StreamProviderPtr<T> source_;
    List state_;
    Iterator current_;
    Iterator end_;

};

} /* namespace provider */
} /* namespace stream */

#endif
