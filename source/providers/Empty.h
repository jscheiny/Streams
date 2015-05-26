#ifndef SCHEINERMAN_STREAM_PROVIDERS_EMPTY_H
#define SCHEINERMAN_STREAM_PROVIDERS_EMPTY_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T>
class empty {

public:
    using element = T;

    std::shared_ptr<element> get() {
        return nullptr;
    }

    bool advance() {
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        this->print_indent(os, indent);
        os << "[empty stream]\n";
        return print_info::source();
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
