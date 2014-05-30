#ifndef STREAM_BOOL_SPECIALIZATION_H
#define STREAM_BOOL_SPECIALIZATION_H

#include <functional>

template<typename T>
class StreamImpl<T, StreamTag::Bool> 
        : public StreamImpl<T, StreamTag::Common> {

private:
    using Super = StreamImpl<T, StreamTag::Common>;

public:
    using ElementType = T;
    using iterator = typename StreamProvider<T>::Iterator;

    StreamImpl() : Super() {}

    template<typename Iterator>
    StreamImpl(Iterator begin, Iterator end) : Super(begin, end) {}

    template<typename Container>
    StreamImpl(const Container& cont) : Super(cont) {}

    decltype(auto) filter()     { return Super::filter(to_bool); }
    decltype(auto) filter_not() { return Super::filter(not_(to_bool)); }
    decltype(auto) take_while() { return Super::take_while(to_bool); }
    decltype(auto) drop_while() { return Super::drop_while(to_bool); }

    decltype(auto) any()  { return Super::any(to_bool); }
    decltype(auto) all()  { return Super::all(to_bool); }
    decltype(auto) none() { return Super::none(to_bool); }

    decltype(auto) not_all() {
        this->check_vacant("not_all");
        return !Super::all(to_bool);
    }

    template<typename, StreamTag>
    friend class StreamImpl;

    friend class MakeStream;

    template<typename, typename, typename>
    friend class FlatMappedStreamProvider;

private:
    StreamImpl(StreamProviderPtr<T> source) : ParentStream(std::move(source)) {}
};

#undef OPERATOR_OVERRIDE

#endif