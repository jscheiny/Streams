#ifndef PROVIDERS_MAP_H
#define PROVIDERS_MAP_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Transform, typename In>
class Map : public StreamProvider<T> {

public:
    Map(StreamProviderPtr<In> source, Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(source_->advance()) {
            current_ = std::make_shared<T>(transform_(std::move(*source_->get())));
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

} /* namespace provider */
} /* namespace stream */

#endif
