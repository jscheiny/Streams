#ifndef SCHEINERMAN_STREAM_PROVIDERS_UNION_H
#define SCHEINERMAN_STREAM_PROVIDERS_UNION_H

#include "StreamProvider.h"
#include "../Utility.h"

#include "SetOperation.h"

namespace stream {
namespace provider {

namespace detail {

template<typename Interface>
class union_impl {

public:
    UpdateState if_neither_depleted(Interface& interface) {
        if (interface.current_left_smaller()) {
            interface.set_advance(ToAdvance::Left);
            interface.set_result(interface.get_current_left());
        } else if (interface.current_right_smaller()) {
            interface.set_advance(ToAdvance::Right);
            interface.set_result(interface.get_current_right());
        } else {
            interface.set_advance(ToAdvance::Both);
            interface.set_result(interface.get_current_left());
        }
        return UpdateState::UpdateFinished;
    }

    std::string get_operation_name() const {
        return "Union";
    }
};

} /* namespace detail */

template<typename Left, typename Right, typename Compare>
using union_ = set_operation<Left, Right, Compare, detail::union_impl>;

} /* namespace provider */
} /* namespace stream */

#endif
