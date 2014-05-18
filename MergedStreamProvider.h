#ifndef MERGED_STREAM_PROVIDER_H
#define MERGED_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

#include "SetOperationStreamProvider.h"

template<typename T, typename Compare>
class MergedStreamProvider : public SetOperationStreamProvider<T, Compare> {

    using Parent = SetOperationStreamProvider<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    MergedStreamProvider(StreamProviderPtr<T>&& source1,
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

    std::string get_operation_name() override {
        return "Merge";
    }

};

#endif
