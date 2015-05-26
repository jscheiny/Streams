#ifndef SCHEINERMAN_STREAM_PROVIDER_H
#define SCHEINERMAN_STREAM_PROVIDER_H

#include "../StreamError.h"
#include "../Utility.h"

#include <memory>

namespace stream {
namespace provider {

struct print_info {
    print_info(int sources_, int stages_)
        : sources(sources_), stages(stages_) {}

    static print_info source() {
        return {1, 0};
    }

    print_info operator+ (const print_info& info) {
        return {sources + info.sources, stages + info.stages};
    }

    print_info add_stage(int stages = 1) {
        return {sources, this->stages + stages};
    }

    int sources;
    int stages;
};

template<typename Provider>
bool stream_advance(std::unique_ptr<Provider>& provider) {
    try {
        return provider->advance();
    } catch (stream::StopStream& stop) {
        return false;
    } catch (...) {
        throw;
    }
}

void print_indent(std::ostream& os, int indent) {
    for (int i = 0; i < indent - 1; i++) {
        os << "  ";
    }
    if (indent != 0) {
        os << "> ";
    }
}

template<typename Provider, typename... Args>
Stream<Provider> make_stream(Args&&... args) {
    return { std::make_unique<Provider>(std::forward<Args>(args)...) };
}

GENERATE_METHOD_CHECK(get, /* no qualifiers */);
GENERATE_METHOD_CHECK(advance, /* no qualifiers */);
GENERATE_METHOD_CHECK(print, const);

} /* namespace provider */
} /* namespace stream */

#include "StreamProviderIterator.h"

#endif
