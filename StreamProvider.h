#ifndef PROVIDER_H
#define PROVIDER_H

#include "StreamError.h"

#include <memory>

template<typename T>
struct StreamProvider {
public:
    virtual std::shared_ptr<T> get() = 0;
    virtual bool advance() = 0;

    class Iterator;

    Iterator begin() { return Iterator::begin(this); }
    Iterator end() { return Iterator::end(this); }
};

template<typename T>
using StreamProviderPtr = std::unique_ptr<StreamProvider<T>>;

template<template<typename...> class Provider,
         typename T,
         typename... TemplateArgs,
         typename... ConstructorArgs>
StreamProviderPtr<T> make_stream_provider(ConstructorArgs&&... args) {
    return StreamProviderPtr<T>(
        new Provider<T, TemplateArgs...>(
            std::forward<ConstructorArgs>(args)...));
}

template<typename T>
class StreamProvider<T>::Iterator {

public:

    T& operator* () {
        check_initial();
        return *current_;
    }

    Iterator& operator++() {
        check_initial();
        check_consumed("prefix increment");
        if(source_->advance()) {
            current_ = source_->get();
        } else {
            state_ = State::End;
            current_.reset();
        }
        return *this;
    }

    Iterator& operator++(int) {
        check_initial();
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

    bool operator== (Iterator& other) {
        check_initial();
        check_consumed("equality check");
        other.check_initial();
        other.check_consumed();
        if(state_ == State::End) {
            return other.state_ == State::End;
        }
        if(other.state_ == State::End) {
            return false;
        }
        return source_ == other.source_ && current_ == other.current_;
    }

    bool operator!= (Iterator& other) {
        check_initial();
        check_consumed("inequality check");
        other.check_initial();
        other.check_consumed("inequality check");
        if(state_ == State::End) {
            return other.state_ != State::End;
        }
        if(other.state_ == State::End) {
            return true;
        }
        return source_ != other.source_ || current_ != other.current_;
    }

    friend class StreamProvider<T>;

private:
    enum class State {
        Initial,
        Consumed,
        Iterating,
        End
    };

    static Iterator begin(StreamProvider<T>* source) {
        return Iterator(source, State::Initial, nullptr);
    }

    static Iterator iterating(StreamProvider<T>* source,
                              std::shared_ptr<T> value) {
        return Iterator(source, State::Iterating, value);
    }

    static Iterator end(StreamProvider<T>* source) {
        return Iterator(source, State::End, nullptr);
    }

    Iterator(StreamProvider<T>* source, State state, std::shared_ptr<T> value)
          : source_(source), state_(state), current_(value) {}

    StreamProvider<T>* source_;
    State state_;
    std::shared_ptr<T> current_;

    void check_initial() {
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

    void check_consumed(const std::string& op) {
        if(state_ == State::Consumed) {
            throw ConsumsedIteratorException(op);
        }
    }

};



#endif
