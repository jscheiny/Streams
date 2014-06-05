#ifndef PROVIDERS_EMPTY_H
#define PROVIDERS_EMPTY_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T>
class Empty : public StreamProvider<T> {

public:
    std::shared_ptr<T> get() override {
        return nullptr;
    }

    bool advance_impl() override {
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "[empty stream]\n";
        return {0, 1};
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
