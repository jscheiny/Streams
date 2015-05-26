#ifndef SCHEINERMAN_STREAM_PROVIDERS_MAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_MAP_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename Source, typename Transform>
class map {

private:
    using source_elem = typename Source::element;

public:
    using element = std::result_of_t<Transform(source_elem&&)>;
    static_assert(!std::is_void<element>::value,
        "Return type of the transform function cannot be void.");

    map(std::unique_ptr<Source> source, Transform&& transform)
        : source_(std::move(source))
        , transform_(transform) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (stream_advance(source_)) {
            current_ = std::make_shared<element>(transform_(std::move(*source_->get())));
            return true;
        }
        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "Map:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Transform transform_;
    std::shared_ptr<element> current_;
};

} /* namespace provider */
} /* namespace stream */

#endif
