#ifndef SCHEINERMAN_STREAM_PROVIDERS_DIFFERENCE_H
#define SCHEINERMAN_STREAM_PROVIDERS_DIFFERENCE_H

#include "SetOperation.h"

namespace stream {
namespace provider {

template<typename T, typename Compare>
class Difference : public SetOperation<T, Compare> {

    using Parent = SetOperation<T, Compare>;
    using UpdateState = typename Parent::UpdateState;
    using ToAdvance = typename Parent::ToAdvance;

public:
    Difference(StreamProviderPtr<T>&& source1,
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

    std::string get_operation_name() const override {
        return "Difference";
    }

private:
    bool source1_advanced_;

};

} /* namespace provider */
} /* namespace stream */

#endif
