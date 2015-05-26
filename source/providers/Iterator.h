#ifndef SCHEINERMAN_STREAM_PROVIDERS_ITERATOR_H
#define SCHEINERMAN_STREAM_PROVIDERS_ITERATOR_H

#include "StreamProvider.h"

#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Itr>
class iterator {

public:
    using element = T;

    iterator(Itr begin, Itr end)
        : current_(begin)
        , end_(end) {}

    std::shared_ptr<element> get() {
        return std::make_shared<element>(std::move(*current_));
    }

    bool advance() {
        if(first_) {
            first_ = false;
            return current_ != end_;
        }
        ++current_;
        return current_ != end_;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "[iterator stream]\n";
        return print_info::source();
    }

private:
    bool first_ = true;
    Itr current_;
    Itr end_;

};

} /* namespace provider */
} /* namespace stream */

#endif
