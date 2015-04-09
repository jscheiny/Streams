#ifndef SCHEINERMAN_STREAM_PROVIDERS_SYMMETRIC_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_SYMMETRIC_DIFFERENCE_H

#include "SetOperation.h"

namespace stream {
namespace provider {

template<typename T, typename Compare>
class SymmetricDifference : public SetOperation<T, Compare> {

    using Parent = SetOperation<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    SymmetricDifference(StreamProviderPtr<T>&& source1,
                        StreamProviderPtr<T>&& source2,
                        Compare&& comparator)
          : Parent(std::forward<StreamProviderPtr<T>>(source1),
                   std::forward<StreamProviderPtr<T>>(source2),
                   std::forward<Compare>(comparator)) {}

protected:
    UpdateState if_neither_depleted() override {
        if(this->current1_smaller()) {
            this->set_advance(ToAdvance::First);
            this->set_result(this->get_current1());
            return UpdateState::UpdateFinished;
        } else if(this->current2_smaller()) {
            this->set_advance(ToAdvance::Second);
            this->set_result(this->get_current2());
            return UpdateState::UpdateFinished;
        } else {
            this->set_advance(ToAdvance::Both);
            return UpdateState::NotFinished;
        }
    }

    std::string get_operation_name() const override {
        return "SymmetricDifference";
    }
};

} /* namespace provider */
} /* namespace stream */

#endif
