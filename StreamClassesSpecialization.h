#ifndef STREAM_CLASSES_SPECIALIZATION_H
#define STREAM_CLASSES_SPECIALIZATION_H

#include <functional>

#define OPERATOR_OVERRIDE(method) \
    template<typename F> \
    auto method (F&& f) \
        { return ParentStream:: method (std::forward<F>(f)); } \
    template<typename R> \
    auto method (R (T::*member)()) \
        { return ParentStream:: method (std::mem_fn(member)); } \
    template<typename R> \
    auto method (R (T::*member)() const) \
        { return ParentStream:: method (std::mem_fn(member)); }


template<typename T>
class StreamImpl<T, true> : public StreamImpl<T, false> {

private:
    using ParentStream = StreamImpl<T, false>;

public:
    using ElementType = T;
    using iterator = typename StreamProvider<T>::Iterator;

    StreamImpl() : ParentStream() {}

    template<typename Iterator>
    StreamImpl(Iterator begin, Iterator end) : ParentStream(begin, end) {}

    template<typename Container>
    StreamImpl(const Container& cont) : ParentStream(cont) {}

    OPERATOR_OVERRIDE(filter);
    OPERATOR_OVERRIDE(take_while);
    OPERATOR_OVERRIDE(drop_while);
    OPERATOR_OVERRIDE(peek);
    OPERATOR_OVERRIDE(map);
    OPERATOR_OVERRIDE(flat_map);

    template<typename, bool>
    friend class StreamImpl;

private:
    StreamImpl(StreamProviderPtr<T> source) : ParentStream(std::move(source)) {}
};

#endif