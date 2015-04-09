#ifndef SCHEINERMAN_STREAM_PROVIDERS_ITERATE_H
#define SCHEINERMAN_STREAM_PROVIDERS_ITERATE_H

#include "StreamProvider.h"

#include "../Utility.h"

#include <iostream>
#include <type_traits>

namespace stream {
namespace provider {

template<typename T, typename Function>
class Iterate : public StreamProvider<T> {

public:
    Iterate(T initial, Function&& function)
        : function_(function), current_(std::make_shared<T>(initial)) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            return true;
        }
        current_ = std::make_shared<T>(function_(*current_));
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[iterated stream]\n";
        return PrintInfo::Source();
    }

private:
    bool first_ = true;
    Function function_;
    std::shared_ptr<T> current_;
};

} /* namespace provider */
} /* namespace stream */

#endif
