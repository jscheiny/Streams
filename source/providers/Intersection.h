#ifndef SCHEINERMAN_STREAM_PROVIDERS_INTERSECTION_H
#define SCHEINERMAN_STREAM_PROVIDERS_INTERSECTION_H

#include "StreamProvider.h"
#include "../Utility.h"

namespace stream {
namespace provider {

template<typename T, typename Compare>
class Intersection : public SetOperation<T, Compare> {

    using Parent = SetOperation<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    Intersection(StreamProviderPtr<T> source1,
                 StreamProviderPtr<T> source2,
                 Compare&& comparator)
          : Parent(std::forward<StreamProviderPtr<T>>(source1),
                   std::forward<StreamProviderPtr<T>>(source2),
                   std::forward<Compare>(comparator)) {}

protected:
    UpdateState if_neither_depleted() override {
        if(this->current1_smaller()) {
            this->set_advance(ToAdvance::First);
        } else if(this->current2_smaller()) {
            this->set_advance(ToAdvance::Second);
        } else {
            this->set_advance(ToAdvance::Both);
            this->set_result(this->get_current1());
            return UpdateState::UpdateFinished;
        }
        return UpdateState::NotFinished;
    }

    virtual UpdateState if_first_depleted() {
        return UpdateState::StreamFinished;
    }

    virtual UpdateState if_second_depleted() {
        return UpdateState::StreamFinished;
    }

    std::string get_operation_name() const override {
        return "Intersection";
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
