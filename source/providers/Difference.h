#ifndef SCHEINERMAN_STREAM_PROVIDERS_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_DIFFERENCE_H

#include "SetOperation.h"

namespace stream {
namespace provider {

namespace detail {

template<typename Interface>
class difference_impl {

public:
    void on_left_source_advance() {
        left_source_advanced_ = true;
    }

    void before_update() {
        left_source_advanced_ = false;
    }

    UpdateState if_neither_depleted(Interface& interface) {
        if (interface.current_left_smaller()) {
            interface.set_advance(ToAdvance::Left);
            interface.set_result(interface.get_current_left());
            if (left_source_advanced_) {
                return UpdateState::UpdateFinished;
            }
        } else if (interface.current_right_smaller()) {
            interface.set_advance(ToAdvance::Right);
        } else {
            interface.set_advance(ToAdvance::Both);
        }
        return UpdateState::NotFinished;
    }

    UpdateState if_left_depleted(Interface&) {
        return UpdateState::StreamFinished;
    }

    std::string get_operation_name() const {
        return "Difference";
    }

private:
    bool left_source_advanced_;

};

} /* namespace detail */

template<typename Left, typename Right, typename Compare>
using difference = set_operation<Left, Right, Compare, detail::difference_impl>;

} /* namespace provider */
} /* namespace stream */

#endif
