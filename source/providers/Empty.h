#ifndef SCHEINERMAN_STREAM_PROVIDERS_EMPTY_H
#define SCHEINERMAN_STREAM_PROVIDERS_EMPTY_H

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

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[empty stream]\n";
        return PrintInfo::Source();
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
