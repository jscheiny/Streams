#ifndef SCHEINERMAN_STREAM_PROVIDERS_ITERATOR_H
#define SCHEINERMAN_STREAM_PROVIDERS_ITERATOR_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Itr>
class Iterator : public StreamProvider<T> {

public:
    Iterator(Itr begin, Itr end)
        : current_(begin), end_(end) {}

    std::shared_ptr<T> get() override {
        return std::make_shared<T>(std::move(*current_));
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            return current_ != end_;
        }
        ++current_;
        return current_ != end_;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[iterator stream]\n";
        return PrintInfo::Source();
    }

private:
    bool first_ = true;
    Itr current_;
    Itr end_;

};

} /* namespace provider */
} /* namespace stream */

#endif
