#ifndef SCHEINERMAN_STREAM_PROVIDERS_SINGLETON_H
#define SCHEINERMAN_STREAM_PROVIDERS_SINGLETON_H

#include "StreamProvider.h"

#include "../Utility.h"

#include <iostream>
#include <type_traits>

namespace stream {
namespace provider {

template<typename T>
class singleton {

public:
    using element = T;

    singleton(const element& value)
        : value_(std::make_shared<element>(value)) {}

    singleton(element&& value)
        : value_(std::make_shared<element>(value)) {}

    std::shared_ptr<element> get() {
        return value_;
    }

    bool advance() {
        if (first_) {
            first_ = false;
            return true;
        }
        value_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        this->print_indent(os, indent);
        os << "[singleton stream]\n";
        return print_info::source();
    }

private:
    bool first_ = true;
    std::shared_ptr<element> value_;
};

} /* namespace provider */
} /* namespace stream */

#endif
