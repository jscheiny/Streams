#ifndef MAPPED_STREAM_PROVIDER_H
#define MAPPED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T, typename Transform, typename In>
class MappedStreamProvider : public StreamProvider<T> {

public:
    MappedStreamProvider(StreamProviderPtr<In> source,
                         Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(source_->advance()) {
            auto preimage = source_->get();
            current_ = std::make_shared<T>(transform_(*preimage));
            return true;
        }
        current_.reset();
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "Map:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<In> source_;
    Transform transform_;
    std::shared_ptr<T> current_;

};

#endif
