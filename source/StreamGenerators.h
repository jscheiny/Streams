#ifndef SCHEINERMAN_STREAM_STREAM_GENERATORS_H
#define SCHEINERMAN_STREAM_STREAM_GENERATORS_H

namespace stream {

namespace detail {

template<template<typename> class Distribution, typename Engine, typename T>
struct RandomGenerator;

} /* namespace detail */

template<typename T>
Stream<RemoveRef<T>> MakeStream::empty() {
    return {};
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<provider::Repeat, R>
        (std::forward<T>(value));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::repeat(T&& value, size_t times) {
    return MakeStream::repeat(value) | op::limit(times);
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::cycle(Iterator begin, Iterator end) {
    using T = IteratorType<Iterator>;
    return MakeStream::repeat(make_pair(begin, end))
        | op::flat_map(splat([](Iterator b, Iterator e) {
            return MakeStream::from(b, e);
        }));
}

template<typename Iterator>
Stream<IteratorType<Iterator>> MakeStream::cycle(Iterator begin, Iterator end,
                                                 size_t times) {
    using T = IteratorType<Iterator>;
    return MakeStream::repeat(make_pair(begin, end), times)
         | op::flat_map(splat([](Iterator b, Iterator e) {
            return MakeStream::from(b, e);
        }));
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(const Container& cont,
                                                   size_t times) {
    return MakeStream::cycle(std::begin(cont), std::end(cont), times);
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle(const Container& cont) {
    return MakeStream::cycle(std::begin(cont), std::end(cont));
}

template<typename T>
Stream<T> MakeStream::cycle(std::initializer_list<T> init) {
    using Container = std::deque<T>;
    return make_stream_provider<provider::CycledContainer, T, Container>
        (Container(init), 0);
}

template<typename T>
Stream<T> MakeStream::cycle(std::initializer_list<T> init, size_t times) {
    using Container = std::deque<T>;
    return make_stream_provider<provider::CycledContainer, T, Container>
        (Container(init), times);
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle_move(Container&& cont) {
    return MakeStream::generate(
        [container = std::move(cont)] () {
            return container;
        })
        | op::flat_map([](const Container& cont) {
            return MakeStream::from(cont);
        });
}

template<typename Container>
Stream<ContainerType<Container>> MakeStream::cycle_move(Container&& cont,
                                                        size_t times) {
    using T = ContainerType<Container>;
    if(times == 0)
        return MakeStream::empty<T>();
    return MakeStream::generate(
        [container = std::move(cont)] () {
            return container;
        })
        | op::limit(times)
        | op::flat_map([](const Container& cont) {
            return MakeStream::from(cont);
        });
}

template<typename Generator>
Stream<std::result_of_t<Generator()>> MakeStream::generate(Generator&& generator) {
    using T = std::result_of_t<Generator()>;
    return make_stream_provider<provider::Generate, T, Generator>
        (std::forward<Generator>(generator));
}

template<typename T, typename Function>
Stream<RemoveRef<T>> MakeStream::iterate(T&& initial, Function&& function) {
    return recurrence(std::forward<Function>(function), std::forward<T>(initial));
}

template<typename... Args, typename Function>
Stream<RemoveRef<Head<Args...>>> MakeStream::recurrence(Function&& function, Args&&... initial) {
    constexpr size_t Order = sizeof...(Args);
    using R = RemoveRef<Head<Args...>>;
    return StreamProviderPtr<R>(
        new provider::Recurrence<R, Order, Function>(
            {{std::forward<Args>(initial)...}}, std::forward<Function>(function)));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::counter(T&& start) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start), [](R value) {
            return ++value;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, U&& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [inc = std::forward<U>(increment)](const R& value) {
            return value + inc;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::counter(T&& start, const U& increment) {
    using R = RemoveRef<T>;
    return MakeStream::iterate(std::forward<T>(start),
        [&inc = increment](const R& value) {
            return value + inc;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::range(T&& lower, T&& upper) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower)
        | op::take_while([upper = std::forward<T>(upper)](const R& value) {
            return value != upper;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::range(T&& lower, T&& upper, U&& increment) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower, std::forward<U>(increment))
        | op::take_while([upper = std::forward<T>(upper)](const R& value) {
            return value < upper;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::range(T&& lower, T&& upper, const U& increment) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower, increment)
        | op::take_while([upper = std::forward<T>(upper)](const R& value) {
            return value < upper;
        });
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::closed_range(T&& lower, T&& upper) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower)
        | op::take_while([upper = std::forward<T>(upper)](R& value) {
            return value <= upper;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::closed_range(T&& lower, T&& upper, U&& increment) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower, std::forward<U>(increment))
        | op::take_while([upper = std::forward<T>(upper)](R& value) {
            return value <= upper;
        });
}

template<typename T, typename U>
Stream<RemoveRef<T>> MakeStream::closed_range(T&& lower, T&& upper, const U& increment) {
    using R = RemoveRef<T>;
    return MakeStream::counter(lower, increment)
        | op::take_while([upper = std::forward<T>(upper)](R& value) {
            return value <= upper;
        });
}

template<typename T,
         template<typename> class Distribution,
         typename Engine,
         typename Seed,
         typename... GenArgs>
Stream<T> MakeStream::randoms_seeded(Seed&& seed, GenArgs&&... args) {
    return generate(detail::RandomGenerator<Distribution, Engine, T>
        (seed, std::forward<GenArgs>(args)...));
}

template<typename T,
         template<typename> class Distribution,
         typename Engine,
         typename... GenArgs>
Stream<T> MakeStream::randoms(GenArgs&&... args) {
    return randoms_seeded<T, Distribution, Engine>
        (default_seed(), std::forward<GenArgs>(args)...);
}

template<typename Engine, typename T>
Stream<T> MakeStream::uniform_random_ints(T lower, T upper) {
    return uniform_random_ints<Engine, T>(lower, upper, default_seed());
}

template<typename Engine, typename T, typename Seed>
Stream<T> MakeStream::uniform_random_ints(T lower, T upper, Seed&& seed) {
    return randoms_seeded<T, std::uniform_int_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename Engine, typename T>
Stream<T> MakeStream::uniform_random_reals(T lower, T upper) {
    return uniform_random_reals<Engine, T>(lower, upper, default_seed());
}

template<typename Engine, typename T, typename Seed>
Stream<T> MakeStream::uniform_random_reals(T lower, T upper, Seed&& seed) {
    return randoms_seeded<T, std::uniform_real_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename Engine, typename T>
Stream<T> MakeStream::normal_randoms(T mean, T stddev) {
    return normal_randoms<Engine, T>(mean, stddev, default_seed());
}

template<typename Engine, typename T, typename Seed>
Stream<T> MakeStream::normal_randoms(T mean, T stddev, Seed&& seed) {
    return randoms_seeded<T, std::normal_distribution, Engine, Seed>
        (std::forward<Seed>(seed), mean, stddev);
}

template<typename Engine, typename T>
Stream<T> MakeStream::coin_flips() {
    return uniform_random_ints<Engine, T>(0, 1);
}

template<typename Engine, typename T, typename Seed>
Stream<T> MakeStream::coin_flips(Seed&& seed) {
    return uniform_random_ints<Engine, T>(0, 1, std::forward<Seed>(seed));
}

template<typename T>
Stream<RemoveRef<T>> MakeStream::singleton(T&& value) {
    using R = RemoveRef<T>;
    return make_stream_provider<provider::Singleton, R>
        (std::forward<T>(value));
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
Stream<ContainerType<Container>> MakeStream::from_move(Container&& cont) {
    using T = ContainerType<Container>;
    return MakeStream::cycle_move(std::forward<Container>(cont), 1);
}

template<typename T>
Stream<T> MakeStream::from(std::initializer_list<T> init) {
    using Container = std::deque<T>;
    return make_stream_provider<provider::CycledContainer, T, Container>
        (Container(init), 1);
}

namespace detail {

template<template<typename> class Distribution, typename Engine, typename T>
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

} /* namespace stream */

#endif
