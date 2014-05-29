#ifndef STREAM_BOOL_SPECIALIZATION_H
#define STREAM_BOOL_SPECIALIZATION_H

#include <functional>


template<typename T>
class StreamImpl<T, StreamTag::Bool> 
        : public StreamImpl<T, StreamTag::Common> {

private:
    using ParentStream = StreamImpl<T, StreamTag::Common>;

public:
    using ElementType = T;
    using iterator = typename StreamProvider<T>::Iterator;

    StreamImpl() : ParentStream() {}

    template<typename Iterator>
    StreamImpl(Iterator begin, Iterator end) : ParentStream(begin, end) {}

    template<typename Container>
    StreamImpl(const Container& cont) : ParentStream(cont) {}

    decltype(auto) filter() {
        return ParentStream::filter(to_bool);
    }
    
    decltype(auto) filter_not() {
        return ParentStream::filter(not_(to_bool));
    }

    decltype(auto) take_while() {
        return ParentStream::take_while(to_bool);
    }

    decltype(auto) drop_while() {
        return ParentStream::drop_while(to_bool);
    }

    decltype(auto) any() {
        return ParentStream::any(to_bool);
    }

    decltype(auto) all() {
        return ParentStream::all(to_bool);
    }

    decltype(auto) none() {
        return ParentStream::none(to_bool);
    }

    decltype(auto) not_all() {
        this->check_vacant("not_all");
        return !ParentStream::all(to_bool);
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