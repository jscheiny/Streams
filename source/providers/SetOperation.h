#ifndef SCHEINERMAN_STREAM_PROVIDERS_SET_OPERATION_H
#define SCHEINERMAN_STREAM_PROVIDERS_SET_OPERATION_H

#include "StreamProvider.h"

namespace stream {
namespace provider {

template<typename T, typename Compare>
class SetOperation : public StreamProvider<T> {

public:
    SetOperation(StreamProviderPtr<T> source1,
                 StreamProviderPtr<T> source2,
                 Compare&& comparator)
          : comparator_(comparator),
            source1_(std::move(source1)),
            source2_(std::move(source2)) {}

    std::shared_ptr<T> get() override {
        return result_;
    }

    bool advance_impl() override {
        if(depletion_ == DepleteState::Both) {
            return false;
        }

        return perform_update();
    }

    PrintInfo print(std::ostream& os, int indent) const override {
        this->print_indent(os, indent);
        os << get_operation_name() << ":\n";
        return source1_->print(os, indent + 1) +
               source2_->print(os, indent + 1);
    }

protected:
    enum class ToAdvance {
        First,
        Second,
        Both
    };

    enum class DepleteState {
        Neither,
        First,
        Second,
        Both
    };

    enum class UpdateState {
        NotFinished,
        UpdateFinished,
        StreamFinished
    };

    std::shared_ptr<T> get_current1() {
        return current1_;
    }

    std::shared_ptr<T> get_current2() {
        return current2_;
    }

    void set_result(std::shared_ptr<T> result) {
        result_ = result;
    }

    void set_advance(ToAdvance advance) {
        advance_ = advance;
    }

    void set_deplete(DepleteState depletion) {
        depletion_ = depletion;
    }

    bool current1_smaller() {
        return comparator_(*current1_, *current2_);
    }

    bool current2_smaller() {
        return comparator_(*current2_, *current1_);
    }

    virtual void on_first_source_advance() {}
    virtual void on_second_source_advance() {}

    virtual void before_update() {}

    virtual UpdateState if_neither_depleted() = 0;

    virtual UpdateState if_first_depleted() {
        set_result(current2_);
        return UpdateState::UpdateFinished;
    }

    virtual UpdateState if_second_depleted() {
        set_result(current1_);
        return UpdateState::UpdateFinished;
    }

    virtual UpdateState if_both_depleted() {
        return UpdateState::StreamFinished;
    }

    virtual std::string get_operation_name() const = 0;

private:
    ToAdvance advance_ = ToAdvance::Both;
    DepleteState depletion_ = DepleteState::Neither;

    Compare comparator_;
    StreamProviderPtr<T> source1_;
    StreamProviderPtr<T> source2_;
    std::shared_ptr<T> current1_;
    std::shared_ptr<T> current2_;
    std::shared_ptr<T> result_;
    bool allow_duplicates_;

    void advance_stream(StreamProviderPtr<T>& source,
                        std::shared_ptr<T>& current,
                        DepleteState deplete_state,
                        ToAdvance advance_result) {
        if(source->advance()) {
            current = source->get();
        } else {
            current.reset();
            if(depletion_ == DepleteState::Neither) {
                depletion_ = deplete_state;
                advance_ = advance_result;
            } else {
                depletion_ = DepleteState::Both;
            }
        }
    }

    void advance_first() {
        advance_stream(source1_, current1_, DepleteState::First,
                       ToAdvance::Second);
        on_first_source_advance();
    }

    void advance_second() {
        advance_stream(source2_, current2_, DepleteState::Second,
                       ToAdvance::First);
        on_second_source_advance();
    }

    void perform_advance() {
        switch(advance_) {
        case ToAdvance::First:
            advance_first();
            break;
        case ToAdvance::Second:
            advance_second();
            break;
        case ToAdvance::Both:
            advance_first();
            advance_second();
            break;
        }
    }

    bool perform_update() {
        before_update();
        while(true) {
            perform_advance();
            UpdateState state;
            if(depletion_ == DepleteState::Neither) {
                state = if_neither_depleted();
            } else if(depletion_ == DepleteState::First) {
                state = if_first_depleted();
            } else if(depletion_ == DepleteState::Second) {
                state = if_second_depleted();
            } else { // depletion_ == DepleteState::Both
                state = if_both_depleted();
            }

            if(state == UpdateState::UpdateFinished) {
                return true;
            } else if(state == UpdateState::StreamFinished) {
                return false;
            } else if(state == UpdateState::NotFinished) {
                continue;
            }
        }
    }

};

} /* namespace provider */
} /* namespace stream */

#endif
