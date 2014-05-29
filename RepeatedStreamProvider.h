#ifndef REPEATED_STREAM_PROVIDER_H
#define REPEATED_STREAM_PROVIDER_H

#include "StreamProvider.h"

#include "Utility.h"

template<typename T>
class RepeatedStreamProvider : public StreamProvider<T> {

public:
    RepeatedStreamProvider(const T& value)
        : value_(std::make_shared<T>(value)) {}

    RepeatedStreamProvider(T&& value)
        : value_(std::make_shared<T>(value)) {}

    std::shared_ptr<T> get() override {
        return value_;
    }

    bool advance_impl() override {
        return true;
    }

    std::pair<int, int> print(std::ostream& os, int indent) const override {
        this->print_indent_arrow(os, indent);
        os << "[repeated value stream]\n";
        return {0, 1};
    }

private:
    std::shared_ptr<T> value_;

};


#endif
