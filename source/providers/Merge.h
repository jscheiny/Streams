#ifndef SCHEINERMAN_STREAM_PROVIDERS_MERGE_H
#define SCHEINERMAN_STREAM_PROVIDERS_MERGE_H

#include "StreamProvider.h"
#include "../Utility.h"

#include "SetOperation.h"

namespace stream {
namespace provider {

template<typename T, typename Compare>
class Merge : public SetOperation<T, Compare> {

    using Parent = SetOperation<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    Merge(StreamProviderPtr<T>&& source1,
          StreamProviderPtr<T>&& source2,
          Compare&& comparator)
          : Parent(std::forward<StreamProviderPtr<T>>(source1),
                   std::forward<StreamProviderPtr<T>>(source2),
                   std::forward<Compare>(comparator)) {}

protected:
    // duplicates
    UpdateState if_neither_depleted() override {
        if(this->current1_smaller()) {
            this->set_advance(ToAdvance::First);
            this->set_result(this->get_current1());
        } else {
            this->set_advance(ToAdvance::Second);
            this->set_result(this->get_current2());
        }
        return UpdateState::UpdateFinished;
    }

    std::string get_operation_name() const override {
        return "Merge";
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
