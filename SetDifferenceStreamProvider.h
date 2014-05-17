#ifndef SET_DIFFERENCE_STREAM_PROVIDER_H
#define SET_DIFFERENCE_STREAM_PROVIDER_H

#include "SetOperationStreamProvider.h"

template<typename T, typename Compare>
class SetDifferenceStreamProvider
          : public SetOperationStreamProvider<T, Compare> {

    using Parent = SetOperationStreamProvider<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    SetDifferenceStreamProvider(StreamProviderPtr<T>&& source1,
                                StreamProviderPtr<T>&& source2,
                                Compare&& comparator)
          : Parent(std::forward<StreamProviderPtr<T>>(source1),
                   std::forward<StreamProviderPtr<T>>(source2),
                   std::forward<Compare>(comparator)) {}

protected:
    void on_first_source_advance() override {
        source1_advanced_ = true;
    }

    void before_update() override {
        source1_advanced_ = false;
    }

    UpdateState if_neither_depleted() override {
        if(this->current1_smaller()) {
            this->set_advance(ToAdvance::First);
            this->set_result(this->get_current1());
            if(source1_advanced_)
                return UpdateState::UpdateFinished;
        } else if(this->current2_smaller()) {
            this->set_advance(ToAdvance::Second);
        } else {
            this->set_advance(ToAdvance::Both);
        }
        return UpdateState::NotFinished;
    }

    UpdateState if_first_depleted() override {
        return UpdateState::StreamFinished;
    }

private:
    bool source1_advanced_;

};

#endif