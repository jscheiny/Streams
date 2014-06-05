#ifndef PROVIDERS_FLAT_MAP_H
#define PROVIDERS_FLAT_MAP_H

#include "StreamProvider.h"
#include "../Stream.h"
#include "../Utility.h"

template<typename T, typename Transform, typename In>
class FlatMappedStreamProvider : public StreamProvider<T> {

public:
    FlatMappedStreamProvider(StreamProviderPtr<In> source,
                             Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(!first_ && current_stream_.source_->advance()) {
            current_ = current_stream_.source_->get();
            return true;
        }

        if(first_)
            first_ = false;

        while(source_->advance()) {
            current_stream_ = std::move(transform_(std::move(*source_->get())));
            if(current_stream_.source_->advance()) {
                current_ = current_stream_.source_->get();
                return true;
            }
        }

        current_.reset();
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "FlatMap:\n";
        auto sub = source_->print(os, indent + 1);
        return {sub.first + 1, sub.second};
    }

private:
    StreamProviderPtr<In> source_;
    Transform transform_;
    Stream<T> current_stream_;
    std::shared_ptr<T> current_;
    bool first_ = true;

};

#endif
