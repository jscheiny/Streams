#ifndef STREAM_FACTORIES_IMPL_H
#define STREAM_FACTORIES_IMPL_H

namespace detail {
template<typename T, template<typename> class Distribution, typename Engine>
struct RandomGenerator;
}

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

template<typename T, template<typename> class Distribution, typename Engine, typename Seed, typename... GenArgs>
Stream<T> MakeStream::randoms(Seed&& seed, GenArgs&&... args) {
    return generate(detail::RandomGenerator<T, Distribution, Engine>
        (seed, std::forward<GenArgs>(args)...));
}

template<typename T, typename Engine>
Stream<T> MakeStream::uniform_random_ints(T lower, T upper) {
    return uniform_random_ints<T, Engine>(lower, upper, default_seed());
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::uniform_random_ints(T lower, T upper, Seed&& seed) {
    return randoms<T, std::uniform_int_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename T, typename Engine>
Stream<T> MakeStream::uniform_random_reals(T lower, T upper) {
    return uniform_random_reals<T, Engine>(lower, upper, default_seed());
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::uniform_random_reals(T lower, T upper, Seed&& seed) {
    return randoms<T, std::uniform_real_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::uniform_random_reals(Seed&& seed) {
    return uniform_random_reals<T, Engine>(0.0, 1.0, std::forward<Seed>(seed));
}

template<typename T, typename Engine>
Stream<T> MakeStream::normal_randoms(T mean, T stddev) {
    return normal_randoms<T, Engine>(mean, stddev, default_seed());
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::normal_randoms(T mean, T stddev, Seed&& seed) {
    return randoms<T, std::normal_distribution, Engine, Seed>
        (std::forward<Seed>(seed), mean, stddev);
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::normal_randoms(Seed&& seed) {
    return normal_randoms<T, Engine>(0.0, 1.0, std::forward<Seed>(seed));
}

template<typename T, typename Engine>
Stream<T> MakeStream::coin_flips() {
    return uniform_random_ints<T, Engine>(0, 1);
}

template<typename T, typename Engine, typename Seed>
Stream<T> MakeStream::coin_flips(Seed&& seed) {
    return uniform_random_ints<T, Engine>(0, 1, std::forward<Seed>(seed));
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

namespace detail {

template<typename T, template<typename> class Distribution, typename Engine>
struct RandomGenerator {
    template<typename Seed, typename... GenArgs>
    RandomGenerator(Seed seed, GenArgs&&... args)
        : generator_{static_cast<typename Engine::result_type>(seed)},
          distro_{std::forward<GenArgs>(args)...} {}

    T operator() () {
        return distro_(generator_);
    }

private:
    Engine generator_;
    Distribution<T> distro_;

};

} /* namespace detail */

#endif