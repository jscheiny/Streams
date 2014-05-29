#ifndef EMPTY_STREAM_PROVIDER_H
#define EMPTY_STREAM_PROVIDER_H

#include "StreamProvider.h"

template<typename T>
class EmptyStreamProvider : public StreamProvider<T> {

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

#endif
