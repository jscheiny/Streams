#ifndef LIMITED_STREAM_PROVIDER_H
#define LIMITED_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class LimitedStreamProvider : public StreamProvider<T> {

public:
    LimitedStreamProvider(StreamProviderPtr<T> source, size_t limit)
        : source_(std::move(source)), remaining_(limit) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(remaining_ == 0) {
            current_.reset();
            return false;
        }
        if(source_->advance()) {
            current_ = source_->get();
            remaining_--;
            return true;
        }
        current_.reset();
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "Limit[" << remaining_ << "]:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<T> source_;
    std::shared_ptr<T> current_;
    std::size_t remaining_;

};

#endif
