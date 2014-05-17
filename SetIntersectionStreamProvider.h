#ifndef SET_INTERSECTION_STREAM_PROVIDER_H
#define SET_INTERSECTION_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Compare>
class SetIntersectionStreamProvider
            : public SetOperationStreamProvider<T, Compare> {

    using Parent = SetOperationStreamProvider<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    SetIntersectionStreamProvider(StreamProviderPtr<T> source1,
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

};

#endif
