#ifndef SCHEINERMAN_STREAM_PROVIDERS_MAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_MAP_H

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

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "Map:\n";
        return source_->print(os, indent + 1).addStage();
    }

private:
    StreamProviderPtr<In> source_;
    Transform transform_;
    std::shared_ptr<T> current_;

};

} /* namespace provider */
} /* namespace stream */

#endif
