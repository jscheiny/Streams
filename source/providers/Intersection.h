#ifndef SCHEINERMAN_STREAM_PROVIDERS_INTERSECTION_H
#define SCHEINERMAN_STREAM_PROVIDERS_INTERSECTION_H

#include "StreamProvider.h"
#include "../Utility.h"

namespace stream {
namespace provider {

namespace detail {

template<typename Interface>
class intersection_impl {

public:
    UpdateState if_neither_depleted(Interface& interface) {
        if(interface.current_left_smaller()) {
            interface.set_advance(ToAdvance::Left);
        } else if(interface.current_right_smaller()) {
            interface.set_advance(ToAdvance::Right);
        } else {
            interface.set_advance(ToAdvance::Both);
            interface.set_result(interface.get_current_left());
            return UpdateState::UpdateFinished;
        }
        return UpdateState::NotFinished;
    }

    UpdateState if_left_depleted(Interface&) {
        return UpdateState::StreamFinished;
    }

    UpdateState if_right_depleted(Interface&) {
        return UpdateState::StreamFinished;
    }

    std::string get_operation_name() const {
        return "Intersection";
    }

};

} /* namespace detail */

template<typename Left, typename Right, typename Compare>
using intersection = set_operation<Left, Right, Compare, detail::intersection_impl>;

} /* namespace provider */
} /* namespace stream */

#endif
