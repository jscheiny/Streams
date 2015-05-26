#ifndef SCHEINERMAN_STREAM_PROVIDERS_MERGE_H
#define SCHEINERMAN_STREAM_PROVIDERS_MERGE_H

#include "StreamProvider.h"
#include "../Utility.h"

#include "SetOperation.h"

namespace stream {
namespace provider {

namespace detail {

template<typename Interface>
class merge_impl {

public:
    // duplicates
    UpdateState if_neither_depleted(Interface& interface) {
        if(interface.current_left_smaller()) {
            interface.set_advance(ToAdvance::Left);
            interface.set_result(interface.get_current_left());
        } else {
            interface.set_advance(ToAdvance::Right);
            interface.set_result(interface.get_current_right());
        }
        return UpdateState::UpdateFinished;
    }

    std::string get_operation_name() const {
        return "Merge";
    }

};

} /* namespace detail */

template<typename Left, typename Right, typename Compare>
using merge = set_operation<Left, Right, Compare, detail::merge_impl>;


} /* namespace provider */
} /* namespace stream */

#endif
