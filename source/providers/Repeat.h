#ifndef SCHEINERMAN_STREAM_PROVIDERS_REPEAT_H
#define SCHEINERMAN_STREAM_PROVIDERS_REPEAT_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T>
class Repeat : public StreamProvider<T> {

public:
    Repeat(const T& value) : value_(std::make_shared<T>(value)) {}

    Repeat(T&& value) : value_(std::make_shared<T>(value)) {}

    std::shared_ptr<T> get() override {
        return value_;
    }

    bool advance_impl() override {
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[repeated value stream]\n";
        return PrintInfo::Source();
    }

private:
    std::shared_ptr<T> value_;

};

} /* namespace provider */
} /* namespace stream */

#endif
