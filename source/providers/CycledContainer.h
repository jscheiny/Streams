#ifndef SCHEINERMAN_STREAM_PROVIDERS_CYCLED_CONTAINER_H
#define SCHEINERMAN_STREAM_PROVIDERS_CYCLED_CONTAINER_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Container>
class CycledContainer : public StreamProvider<T> {

public:
    CycledContainer(Container&& container, size_t times)
        : container_{container},
          current_{std::begin(container_)},
          end_{std::end(container_)},
          times_{times} {}

    std::shared_ptr<T> get() override {
        return std::make_shared<T>(std::move(*current_));
    }

    bool advance_impl() override {
        if(first_) {
            first_ = false;
            return current_ != end_;
        }
        ++current_;
        if(current_ == end_) {
            iteration_++;
            if(iteration_ > times_ && times_ != 0)
                return false;
            current_ = std::begin(container_);
        }
        return true;
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << "[cycled container stream]\n";
        return PrintInfo::Source();
    }

private:
    using Iterator = decltype(std::begin(std::declval<Container>()));

    bool first_ = true;
    Container container_;
    Iterator current_;
    Iterator end_;
    size_t times_;
    size_t iteration_ = 1;
};

} /* namespace provider */
} /* namespace stream */

#endif
