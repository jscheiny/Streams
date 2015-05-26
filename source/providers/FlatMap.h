#ifndef SCHEINERMAN_STREAM_PROVIDERS_FLAT_MAP_H
#define SCHEINERMAN_STREAM_PROVIDERS_FLAT_MAP_H

#include "StreamProvider.h"
#include "../Stream.h"
#include "../Utility.h"

namespace stream {
namespace provider {

template<typename Source, typename Transform>
class flat_map {

private:
    using source_elem = typename Source::element;
    using stream_t = std::result_of_t<Transform(source_elem&&)>;
    static_assert(!std::is_void<stream_t>::value, "Return type shouldnt be void");
    static_assert(is_stream<stream_t>,
        "Flat map must be passed a function which returns a stream.");

public:
    using element = typename stream_t::element;

    flat_map(std::unique_ptr<Source> source, Transform&& transform)
        : source_(std::move(source)), transform_(transform) {}

    std::shared_ptr<element> get() {
        return current_;
    }

    bool advance() {
        if (!first_ && stream_advance(current_stream_.source())) {
            current_ = current_stream_.source()->get();
            return true;
        }

        if (first_)
            first_ = false;

        while (stream_advance(source_)) {
            current_stream_ = std::move(transform_(std::move(*source_->get())));
            if (stream_advance(current_stream_.source())) {
                current_ = current_stream_.source()->get();
                return true;
            }
        }

        current_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "FlatMap:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Transform transform_;
    stream_t current_stream_;
    std::shared_ptr<element> current_;
    bool first_ = true;

};

} /* namespace provider */
} /* namespace stream */

#endif
