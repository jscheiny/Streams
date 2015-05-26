#ifndef SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_ADJACENT_DIFFERENCE_H

#include "StreamProvider.h"
#include "../Utility.h"

#include <type_traits>

namespace stream {
namespace provider {

template<typename Source, typename Subtractor>
class adjacent_difference {

private:
    using source_elem = typename Source::element;

public:
    using element = std::result_of_t<Subtractor(source_elem&, source_elem&)>;

    adjacent_difference(std::unique_ptr<Source> source, Subtractor&& subtract)
        : source_(std::move(source))
        , subtract_(subtract) {}

    std::shared_ptr<element> get() {
        return result_;
    }

    bool advance() {
        if (first_advance_) {
            first_advance_ = false;
            if (stream_advance(source_)) {
                first_ = source_->get();
            } else {
                return false;
            }
            if (stream_advance(source_)) {
                second_ = source_->get();
            } else {
                first_.reset();
                return false;
            }
            result_ = std::make_shared<element>(subtract_(*second_, *first_));
            return true;
        }

        first_ = std::move(second_);
        if (stream_advance(source_)) {
            second_ = source_->get();
            result_ = std::make_shared<element>(subtract_(*second_, *first_));
            return true;
        }
        first_.reset();
        result_.reset();
        return false;
    }

    print_info print(std::ostream& os, int indent) const {
        print_indent(os, indent);
        os << "AdjacentDifference:\n";
        return source_->print(os, indent + 1).add_stage();
    }

private:
    std::unique_ptr<Source> source_;
    Subtractor subtract_;
    std::shared_ptr<source_elem> first_;
    std::shared_ptr<source_elem> second_;
    std::shared_ptr<element> result_;
    bool first_advance_ = true;

};

} /* namespace provider */
} /* namespace stream */

#endif
