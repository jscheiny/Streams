#ifndef ITERATED_STREAM_PROVIDER_H
#define ITERATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

#include <iostream>
#include <type_traits>

template<typename T, typename Function>
class IteratedStreamProvider : public StreamProvider<T> {

public:
    IteratedStreamProvider(T initial, Function&& function)
        : function_(function),
          current_(std::make_shared<T>(initial)) {}

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

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "[iterated stream]\n";
        return {0, 1};
    }

private:
    bool first_ = true;
    Function function_;
    std::shared_ptr<T> current_;
};


#endif
