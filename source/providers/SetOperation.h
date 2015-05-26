#ifndef SCHEINERMAN_STREAM_PROVIDERS_SET_OPERATION_H
#define SCHEINERMAN_STREAM_PROVIDERS_SET_OPERATION_H

#include "StreamProvider.h"
#include "../Utility.h"

namespace stream {
namespace provider {

GENERATE_METHOD_CHECK(if_neither_depleted, /* no qualifiers */);
GENERATE_METHOD_CHECK(get_operation_name, const);
GENERATE_METHOD_CHECK(if_left_depleted, /* no qualifiers */);
GENERATE_METHOD_CHECK(if_right_depleted, /* no qualifiers */);
GENERATE_METHOD_CHECK(if_both_depleted, /* no qualifiers */);
GENERATE_METHOD_CHECK(on_left_source_advance, /* no qualifiers */);
GENERATE_METHOD_CHECK(on_right_source_advance, /* no qualifiers */);
GENERATE_METHOD_CHECK(before_update, /* no qualifiers */);

#undef GENERATE_METHOD_CHECK

enum class ToAdvance {
    Left,
    Right,
    Both
};

enum class DepleteState {
    Neither,
    Left,
    Right,
    Both
};

enum class UpdateState {
    NotFinished,
    UpdateFinished,
    StreamFinished
};


template<typename T, typename Compare>
class set_operation_interface;

template<typename LeftSource, typename RightSource, typename Compare,
         template<typename> class Strategy>
class set_operation {

    // Strategy classes for set operation are required to be template classes taking
    // a single template argument (Interface) and must implement the following methods:
    //      UpdateState if_neither_depleted(Interface&);
    //      std::string get_operation_name() const;
    // Optional methods for strategy classes:
    //      void on_left_source_advance(); [default noop]
    //      void on_right_source_advance(); [default noop]
    //      void before_update(); [default noop]
    //      UpdateState if_left_depleted() const;
    //      UpdateState if_right_depleted() const;
    //      UpdateState if_both_depleted() const;

    using left_elem = typename LeftSource::element;
    using right_elem = typename RightSource::element;
    static_assert(std::is_same<left_elem, right_elem>::value,
        "Cannot combine streams of different types using set operations.");

    using Interface = set_operation_interface<left_elem, Compare>;

    static_assert(detail::has_if_neither_depleted<Strategy<Interface>, UpdateState, Interface&>::value,
        "Strategy class must provide the method UpdateState if_neither_depleted(Interface&)");
    static_assert(detail::has_get_operation_name<Strategy<Interface>, std::string>::value,
        "Strategy class must provide the method std::string get_operation_name() const");

public:
    using element = left_elem;

    set_operation(std::unique_ptr<LeftSource> left_source,
                  std::unique_ptr<RightSource> right_source,
                  Compare&& comparator)
          : left_source_{std::move(left_source)}
          , right_source_{std::move(right_source)}
          , interface_{std::forward<Compare>(comparator)} {}

    std::shared_ptr<element> get() {
        return interface_.result_;
    }

    bool advance() {
        if (interface_.depletion_ == DepleteState::Both) {
            return false;
        }

        detail::has_before_update<Strategy<Interface>, void>::eval(strategy_);
        while(true) {
            perform_advance();
            UpdateState state;
            if (interface_.depletion_ == DepleteState::Neither) {
                state = strategy_.if_neither_depleted(interface_);
            } else if (interface_.depletion_ == DepleteState::Left) {
                state = if_left_depleted();
            } else if (interface_.depletion_ == DepleteState::Right) {
                state = if_right_depleted();
            } else { // interface_.depletion_ == DepleteState::Both
                state = if_both_depleted();
            }

            if (state == UpdateState::UpdateFinished) {
                return true;
            } else if (state == UpdateState::StreamFinished) {
                return false;
            } else if (state == UpdateState::NotFinished) {
                continue;
            }
        }
    }

    print_info print(std::ostream& os, int indent) const {
        this->print_indent(os, indent);
        os << strategy_->get_operation_name() << ":\n";
        return left_source_->print(os, indent + 1) +
               right_source_->print(os, indent + 1);
    }

private:
    std::unique_ptr<LeftSource> left_source_;
    std::unique_ptr<RightSource> right_source_;

    Interface interface_;
    Strategy<Interface> strategy_;

    template<typename Source>
    void advance_stream(std::unique_ptr<Source>& source,
                        std::shared_ptr<element>& current,
                        DepleteState deplete_state,
                        ToAdvance advance_result) {
        if (stream_advance(source)) {
            current = source->get();
        } else {
            current.reset();
            if (interface_.depletion_ == DepleteState::Neither) {
                interface_.depletion_ = deplete_state;
                interface_.advance_ = advance_result;
            } else {
                interface_.depletion_ = DepleteState::Both;
            }
        }
    }

    void advance_left() {
        advance_stream(left_source_, interface_.current_left_,
                       DepleteState::Left, ToAdvance::Right);
        detail::has_on_left_source_advance<Strategy<Interface>, void>::eval(strategy_);
    }

    void advance_right() {
        advance_stream(right_source_, interface_.current_right_,
                       DepleteState::Right, ToAdvance::Left);
        detail::has_on_right_source_advance<Strategy<Interface>, void>::eval(strategy_);
    }

    void perform_advance() {
        switch(interface_.advance_) {
        case ToAdvance::Left:
            advance_left();
            break;
        case ToAdvance::Right:
            advance_right();
            break;
        case ToAdvance::Both:
            advance_left();
            advance_right();
            break;
        }
    }

    UpdateState if_left_depleted() {
        return detail::has_if_left_depleted<Strategy<Interface>, UpdateState, Interface&>
        ::eval(strategy_, interface_, [this] {
            interface_.set_result(interface_.current_right_);
            return UpdateState::UpdateFinished;
        });
    }

    UpdateState if_right_depleted() {
        return detail::has_if_right_depleted<Strategy<Interface>, UpdateState, Interface&>
        ::eval(strategy_, interface_, [this] {
            interface_.set_result(interface_.current_left_);
            return UpdateState::UpdateFinished;
        });
    }

    UpdateState if_both_depleted() {
        return detail::has_if_both_depleted<Strategy<Interface>, UpdateState, Interface&>
        ::eval(strategy_, interface_, [] {
            return UpdateState::StreamFinished;
        });
    }


};

template<typename T, typename Compare>
class set_operation_interface {

public:

    std::shared_ptr<T> get_current_left() {
        return current_left_;
    }

    std::shared_ptr<T> get_current_right() {
        return current_right_;
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

    bool current_left_smaller() {
        return comparator_(*current_left_, *current_right_);
    }

    bool current_right_smaller() {
        return comparator_(*current_right_, *current_left_);
    }

    template<typename L, typename R, typename C, template<typename> class S>
    friend class set_operation;

private:
    set_operation_interface(Compare&& comparator)
        : comparator_(comparator) {}

    DepleteState get_deplete() const;
    ToAdvance get_advance() const;

    ToAdvance advance_ = ToAdvance::Both;
    DepleteState depletion_ = DepleteState::Neither;

    Compare comparator_;
    std::shared_ptr<T> current_left_;
    std::shared_ptr<T> current_right_;
    std::shared_ptr<T> result_;

};

} /* namespace provider */
} /* namespace stream */

#endif
