#ifndef SCHEINERMAN_STREAM_PROVIDERS_SYMMETRIC_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_SYMMETRIC_DIFFERENCE_H

#include "SetOperation.h"

namespace stream {
namespace provider {

namespace detail {

template<typename Interface>
class symmetric_difference_impl {

public:
    UpdateState if_neither_depleted(Interface& interface) {
        if (interface.current_left_smaller()) {
            interface.set_advance(ToAdvance::Left);
            interface.set_result(interface.get_current_left());
            return UpdateState::UpdateFinished;
        } else if (interface.current_right_smaller()) {
            interface.set_advance(ToAdvance::Right);
            interface.set_result(interface.get_current_right());
            return UpdateState::UpdateFinished;
        } else {
            interface.set_advance(ToAdvance::Both);
            return UpdateState::NotFinished;
        }
    }

    std::string get_operation_name() const {
        return "SymmetricDifference";
    }
};

} /* namespace detail */

template<typename Left, typename Right, typename Compare>
using symmetric_difference = set_operation<Left, Right, Compare, detail::symmetric_difference_impl>;

} /* namespace provider */
} /* namespace stream */

#endif
