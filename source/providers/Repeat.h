#ifndef SCHEINERMAN_STREAM_PROVIDERS_REPEAT_H
#define SCHEINERMAN_STREAM_PROVIDERS_REPEAT_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T>
class repeat {

public:
    using element = T;

    repeat(const element& value)
        : value_(std::make_shared<element>(value)) {}

    repeat(element&& value)
        : value_(std::make_shared<element>(value)) {}

    std::shared_ptr<element> get() {
        return value_;
    }

    bool advance() {
        return true;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "[repeated value stream]\n";
        return print_info::source();
    }

private:
    std::shared_ptr<element> value_;

};

} /* namespace provider */
} /* namespace stream */

#endif
