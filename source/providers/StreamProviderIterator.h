#ifndef SCHEINERMAN_STREAM_STREAM_PROVIDER_ITERATOR_H
#define SCHEINERMAN_STREAM_STREAM_PROVIDER_ITERATOR_H

namespace stream {
namespace provider {

template<typename T>
struct StreamProvider<T>::Iterator {

public:
    T& operator* ();
    Iterator& operator++ ();
    Iterator operator++ (int);
    bool operator== (Iterator& other);
    bool operator!= (Iterator& other);

    friend struct StreamProvider<T>;

private:
    enum class State {
        Initial,
        Consumed,
        Iterating,
        End
    };

    static Iterator begin(StreamProvider<T>* source);
    static Iterator iterating(StreamProvider<T>* source, std::shared_ptr<T> value);
    static Iterator end(StreamProvider<T>* source);

    Iterator(StreamProvider<T>* source, State state, std::shared_ptr<T> value);

    void update_initial();
    void check_consumed(const std::string& op) const;

    StreamProvider<T>* source_;
    State state_;
    std::shared_ptr<T> current_;

};

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::begin(StreamProvider<T>* source) {
    return Iterator(source, State::Initial, nullptr);
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::iterating(StreamProvider<T>* source,
                                       std::shared_ptr<T> value) {
    return Iterator(source, State::Iterating, value);
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::end(StreamProvider<T>* source) {
    return Iterator(source, State::End, nullptr);
}

template<typename T>
StreamProvider<T>::Iterator::Iterator(StreamProvider<T>* source, State state,
                                      std::shared_ptr<T> value)
      : source_(source), state_(state), current_(value) {}

template<typename T>
T& StreamProvider<T>::Iterator::operator* () {
    update_initial();
    return *current_;
}

template<typename T>
typename StreamProvider<T>::Iterator&
StreamProvider<T>::Iterator::operator++() {
    update_initial();
    check_consumed("prefix increment");
    if(source_->advance()) {
        current_ = source_->get();
    } else {
        state_ = State::End;
        current_.reset();
    }
    return *this;
}

template<typename T>
typename StreamProvider<T>::Iterator
StreamProvider<T>::Iterator::operator++(int) {
    update_initial();
    check_consumed("postfix increment");
    Iterator result(nullptr, State::Consumed, current_);
    if(source_->advance()) {
        current_ = source_->get();
    } else {
        state_ = State::End;
        current_.reset();
    }
    return result;
}

template<typename T>
bool StreamProvider<T>::Iterator::operator== (Iterator& other) {
    update_initial();
    check_consumed("equality check");
    other.update_initial();
    other.check_consumed();
    if(state_ == State::End) {
        return other.state_ == State::End;
    }
    if(other.state_ == State::End) {
        return false;
    }
    return source_ == other.source_ && current_ == other.current_;
}

template<typename T>
bool StreamProvider<T>::Iterator::operator!= (Iterator& other) {
    update_initial();
    check_consumed("inequality check");
    other.update_initial();
    other.check_consumed("inequality check");
    if(state_ == State::End) {
        return other.state_ != State::End;
    }
    if(other.state_ == State::End) {
        return true;
    }
    return source_ != other.source_ || current_ != other.current_;
}

template<typename T>
void StreamProvider<T>::Iterator::update_initial() {
    if(state_ == State::Initial) {
        if(source_->advance()) {
            current_ = source_->get();
            state_ = State::Iterating;
        } else {
            current_.reset();
            state_ = State::End;
        }
    }
}

template<typename T>
void StreamProvider<T>::Iterator::check_consumed(const std::string& op) const {
    if(state_ == State::Consumed) {
        throw stream::ConsumedIteratorException(op);
    }
}

} /* namespace provider */
} /* namespace stream */

#endif
