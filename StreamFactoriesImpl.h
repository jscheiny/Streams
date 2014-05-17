#ifndef STREAM_FACTORIES_IMPL_H
#define STREAM_FACTORIES_IMPL_H

template<typename T>
Stream<RemoveRef<T>> MakeStream::empty() {
    return {};
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<RepeatedStreamProvider, R>
        (std::forward<R>(value));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value, size_t times) {
    return MakeStream::repeat(value).limit(times);
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::cycle(Iterator begin, Iterator end) {
    using T = IteratorType<Iterator>;
    return MakeStream::repeat(make_pair(begin, end))
        .flat_map(splat([](Iterator b, Iterator e) {
            return MakeStream::from(b, e);
        }));
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(const Container& cont) {
    return MakeStream::cycle(std::begin(cont), std::end(cont));
}


template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(Container&& cont) {
    return MakeStream::cycle(std::begin(cont), std::end(cont));
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::cycle(Iterator begin, Iterator end,
                                                 size_t times) {
    using T = IteratorType<Iterator>;
    return MakeStream::repeat(make_pair(begin, end), times)
        .flat_map(splat([](Iterator b, Iterator e) {
            return MakeStream::from(b, e);
        }));
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(const Container& cont,
                                                   size_t times) {
    return MakeStream::cycle(std::begin(cont), std::end(cont), times);
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(Container&& cont,
                                                   size_t times) {
    return MakeStream::cycle(std::begin(cont), std::end(cont), times);
}

template<typename Generator>
Stream<ReturnType<Generator>> MakeStream::generate(Generator&& generator) {
    using T = ReturnType<Generator>;
    return make_stream_provider<GeneratedStreamProvider, T, Generator>
        (std::forward<Generator>(generator));
}

template<typename T, typename Function>
Stream<RemoveRef<T>> MakeStream::iterate(T&& initial, Function&& function) {
    using R = RemoveRef<T>;
    return make_stream_provider<IteratedStreamProvider, R, Function>
        (std::forward<T>(initial), std::forward<Function>(function));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start), [](R& value) {
            return ++value;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, T&& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [inc = std::forward<T>(increment)](R& value) {
            return value + inc;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, const T& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [&inc = increment](R& value) {
            return value + inc;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::singleton(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<SingletonStreamProvider, R>
        (std::forward<R>(value));
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::from(Iterator begin, Iterator end) {
    return {begin, end};
}

template<typename T>
Stream<T> MakeStream::from(T* array, std::size_t length) {
    return Stream<T>(array, array + length);
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::from(const Container& cont) {
    return {std::begin(cont), std::end(cont)};
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::from(Container&& cont) {
    return {std::begin(cont), std::end(cont)};
}

template<typename T>
Stream<T> MakeStream::from(std::initializer_list<T> init) {
    return Stream<T>(init.begin(), init.end());
}

#endif