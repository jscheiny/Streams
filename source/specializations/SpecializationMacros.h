#define PUBLIC_USINGS \
    using ElementType = T; \
    using iterator = typename StreamProvider<T>::Iterator

#define PRIVATE_USINGS \
    using Super = StreamImpl<T, Common>

#define PUBLIC_CONSTRUCTORS \
    StreamImpl() : Super() {} \
    template<typename Iterator> \
    StreamImpl(Iterator begin, Iterator end) : Super(begin, end) {}

#define MOVE_SEMANTICS(Tags) \
    StreamImpl(StreamImpl< T, Tags >&& other) \
        : Super(std::move(other.source_)) {} \
    StreamImpl< T, Tags >& operator= (StreamImpl< T, Tags >&& other) \
        { this->source_ = std::move(other.source_); return *this; }

#define PRIVATE_CONSTRUCTORS \
    StreamImpl(StreamProviderPtr<T> source) : Super(std::move(source)) {}

#define FRIENDS \
    friend class MakeStream; \
    template<typename, int> friend class StreamImpl; \
    template<typename, typename, typename> friend class FlatMappedStreamProvider