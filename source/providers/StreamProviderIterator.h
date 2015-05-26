#ifndef SCHEINERMAN_STREAM_STREAM_PROVIDER_ITERATOR_H
#define SCHEINERMAN_STREAM_STREAM_PROVIDER_ITERATOR_H

namespace stream {
namespace provider {

template<typename Provider> struct provider_iterator;

template<typename Provider>
provider_iterator<Provider> begin(std::unique_ptr<Provider>& provider);

template<typename Provider>
provider_iterator<Provider> end(std::unique_ptr<Provider>& provider);

template<typename Provider>
struct provider_iterator {

private:
    using element = typename Provider::element;

public:
    element& operator* ();
    provider_iterator<Provider>& operator++ ();
    provider_iterator<Provider> operator++ (int);
    bool operator== (provider_iterator<Provider>& other);
    bool operator!= (provider_iterator<Provider>& other);

    template<typename P>
    friend provider_iterator<P> begin(std::unique_ptr<P>& provider);

    template<typename P>
    friend provider_iterator<P> end(std::unique_ptr<P>& provider);

private:
    enum class State {
        Initial,
        Consumed,
        Iterating,
        End
    };

    static provider_iterator<Provider> iterating(Provider* source, std::shared_ptr<element> value);

    provider_iterator(Provider* source, State state, std::shared_ptr<element> value)
        : source_(source), state_(state), current_(value) {}

    void update_initial();
    void check_consumed(const std::string& op) const;

    Provider* source_;
    State state_;
    std::shared_ptr<element> current_;

};

template<typename Provider>
provider_iterator<Provider> begin(std::unique_ptr<Provider>& provider) {
    return {provider.get(), provider_iterator<Provider>::State::Initial, nullptr};
}

template<typename Provider>
provider_iterator<Provider> end(std::unique_ptr<Provider>& provider) {
    return {provider.get(), provider_iterator<Provider>::State::End, nullptr};
}

template<typename Provider>
typename provider_iterator<Provider>::element& provider_iterator<Provider>::operator* () {
    update_initial();
    return *current_;
}

template<typename Provider>
provider_iterator<Provider>&
provider_iterator<Provider>::operator++() {
    update_initial();
    check_consumed("prefix increment");
    if (stream_advance(source_)) {
        current_ = source_->get();
    } else {
        state_ = State::End;
        current_.reset();
    }
    return *this;
}

template<typename Provider>
provider_iterator<Provider>
provider_iterator<Provider>::operator++(int) {
    update_initial();
    check_consumed("postfix increment");
    provider_iterator<Provider> result{nullptr, State::Consumed, current_};
    if (stream_advance(source_)) {
        current_ = source_->get();
    } else {
        state_ = State::End;
        current_.reset();
    }
    return result;
}

template<typename Provider>
bool provider_iterator<Provider>::operator== (provider_iterator<Provider>& other) {
    update_initial();
    check_consumed("equality check");
    other.update_initial();
    other.check_consumed();
    if (state_ == State::End) {
        return other.state_ == State::End;
    }
    if (other.state_ == State::End) {
        return false;
    }
    return source_ == other.source_ && current_ == other.current_;
}

template<typename Provider>
bool provider_iterator<Provider>::operator!= (provider_iterator<Provider>& other) {
    update_initial();
    check_consumed("inequality check");
    other.update_initial();
    other.check_consumed("inequality check");
    if (state_ == State::End) {
        return other.state_ != State::End;
    }
    if (other.state_ == State::End) {
        return true;
    }
    return source_ != other.source_ || current_ != other.current_;
}

template<typename Provider>
void provider_iterator<Provider>::update_initial() {
    if (state_ == State::Initial) {
        if (stream_advance(source_)) {
            current_ = source_->get();
            state_ = State::Iterating;
        } else {
            current_.reset();
            state_ = State::End;
        }
    }
}

template<typename Provider>
void provider_iterator<Provider>::check_consumed(const std::string& op) const {
    if (state_ == State::Consumed) {
        throw stream::ConsumedIteratorException(op);
    }
}

} /* namespace provider */
} /* namespace stream */

#endif
