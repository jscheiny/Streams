#define PUBLIC_USINGS \
    using ElementType = T; \
    using iterator = typename StreamProvider<T>::Iterator

#define PRIVATE_USINGS \
    using Super = StreamImpl<T, Common>

#define PUBLIC_CONSTRUCTORS \
    StreamImpl() : Super() {} \
    template<typename Iterator> \
    StreamImpl(Iterator begin, Iterator end) : Super(begin, end) {} \
    template<typename Container> \
    StreamImpl(const Container& cont) : Super(cont) {} \

#define PRIVATE_CONSTRUCTORS \
    StreamImpl(StreamProviderPtr<T> source) : Super(std::move(source)) {}

#define FRIENDS \
    friend class MakeStream; \
    template<typename, int> friend class StreamImpl; \
    template<typename, typename, typename> friend class FlatMappedStreamProvider