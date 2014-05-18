#ifndef SET_SYMMETRIC_DIFFERENCE_STREAM_PROVIDER_H
#define SET_SYMMETRIC_DIFFERENCE_STREAM_PROVIDER_H

#include "SetOperationStreamProvider.h"

template<typename T, typename Compare>
class SetSymmetricDifferenceStreamProvider 
          : public SetOperationStreamProvider<T, Compare> {
    
    using Parent = SetOperationStreamProvider<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    SetSymmetricDifferenceStreamProvider(StreamProviderPtr<T>&& source1,
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

    std::string get_operation_name() override {
        return "SymmetricDifference";
    }
};

#endif