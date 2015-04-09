#ifndef SCHEINERMAN_STREAM_PROVIDERS_SINGLETON_H
#define SCHEINERMAN_STREAM_PROVIDERS_SINGLETON_H

#include "StreamProvider.h"

#include "../Utility.h"

#include <iostream>
#include <type_traits>

namespace stream {
namespace provider {

template<typename T>
class Singleton : public StreamProvider<T> {

public:
    Singleton(const T& value) : value_(std::make_shared<T>(value)) {}

    Singleton(T&& value) : value_(std::make_shared<T>(value)) {}

    std::shared_ptr<T> get() override {
        return value_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            return true;
        }
        value_.reset();
        return false;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[singleton stream]\n";
        return PrintInfo::Source();
    }

private:
    bool first_ = true;
    std::shared_ptr<T> value_;
};

} /* namespace provider */
} /* namespace stream */

#endif
