#ifndef SINGLETON_STREAM_PROVIDER_H
#define SINGLETON_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

#include <iostream>
#include <type_traits>

template<typename T>
class SingletonStreamProvider : public StreamProvider<T> {

public:
    SingletonStreamProvider(const T& value)
        : value_(std::make_shared<T>(value)) {}

    SingletonStreamProvider(T&& value)
        : value_(std::make_shared<T>(value)) {}

    std::shared_ptr<T> get() override {
        return value_;
    }

    bool advance() override {
        if(first_) {
            first_ = false;
            return true;
        }
        value_.reset();
        return false;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "[singleton stream]\n";
        return {0, 1};
    }

private:
    bool first_ = true;
    std::shared_ptr<T> value_;
};

#endif
