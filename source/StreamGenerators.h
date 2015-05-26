#ifndef SCHEINERMAN_STREAM_STREAM_GENERATORS_H
#define SCHEINERMAN_STREAM_STREAM_GENERATORS_H

namespace stream {
namespace make_stream {

namespace detail {

template<template<typename> class Distribution, typename Engine, typename T>
struct RandomGenerator;

auto default_seed() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

} /* namespace detail */

template<typename T>
auto empty() {
    return provider::make_stream<provider::empty<T>>();
}

template<typename Iterator>
auto from(Iterator begin, Iterator end) {
    using T = IteratorType<Iterator>;
    return provider::make_stream<provider::iterator<T, Iterator>>(begin, end);
}

template<typename T>
auto from(T* array, std::size_t length) {
    return from(array, array + length);
}

template<typename Container>
auto from(const Container& cont) {
    return from(std::begin(cont), std::end(cont));
}

template<typename Generator>
auto generate(Generator&& generator) {
    return provider::make_stream<provider::generate<Generator>>
        (std::forward<Generator>(generator));
}

template<typename T>
auto repeat(T&& value) {
    using R = RemoveRef<T>;
    return provider::make_stream<provider::repeat<R>>(std::forward<T>(value));
}

template<typename T>
auto repeat(T&& value, size_t times) {
    return repeat(value) | op::limit(times);
}

template<typename Iterator>
auto cycle(Iterator begin, Iterator end) {
    return repeat(std::make_pair(begin, end))
        | op::flat_map(splat([](Iterator b, Iterator e) {
            return from(b, e);
        }));
}

template<typename Iterator>
auto cycle(Iterator begin, Iterator end, size_t times) {
    return repeat(std::make_pair(begin, end), times)
        | op::flat_map(splat([](Iterator b, Iterator e) {
            return from(b, e);
        }));
}

template<typename Container>
auto cycle(const Container& cont) {
    return cycle(std::begin(cont), std::end(cont));
}

template<typename Container>
auto cycle(const Container& cont, size_t times) {
    return cycle(std::begin(cont), std::end(cont), times);
}

template<typename T>
auto cycle(std::initializer_list<T> init, size_t times = 0) {
    using Container = std::deque<T>;
    return provider::make_stream<provider::cycled_container<Container>>
        (Container(init), times);
}

template<typename Container>
auto cycle_move(Container&& cont) {
    return generate(
        [container = std::move(cont)] () {
            return container;
        })
        | op::flat_map([](const Container& cont) {
            return make_stream::from(cont);
        });
}

template<typename Container>
auto cycle_move(Container&& cont, size_t times) {
    using T = ContainerType<Container>;
    return generate([container = std::move(cont)] {
            return std::make_pair(std::begin(container), std::end(container));
        })
        | op::limit(times)
        | op::flat_map([](const auto& pair) {
            return from(pair.first, pair.second);
        });
}

template<typename... Args, typename Function>
auto recurrence(Function&& function, Args&&... initial) {
    constexpr size_t Order = sizeof...(Args);
    using R = RemoveRef<Head<Args...>>;
    return provider::make_stream<provider::recurrence<R, Order, Function>>
        (std::array<R, Order>{{std::forward<Args>(initial)...}},
         std::forward<Function>(function));
}

template<typename T, typename Function>
auto iterate(T&& initial, Function&& function) {
    return recurrence(std::forward<Function>(function), std::forward<T>(initial));
}

template<typename T>
auto counter(const T& start) {
    return iterate(start, [] (auto value) {
            return ++value;
        });
}

template<typename T, typename U>
auto counter(const T& start, const U& increment) {
    return iterate(start, [increment] (const auto& value) {
            return value + increment;
        });
}

template<typename T>
auto range(const T& lower, const T& upper) {
    return counter(lower)
        | op::take_while([upper] (const auto& value) {
            return value != upper;
        });
}

template<typename T, typename U>
auto range(const T& lower, const T& upper, const U& increment) {
    return counter(lower, increment)
        | op::take_while([upper] (const auto& value) {
            return value < upper;
        });
}

template<typename T>
auto closed_range(const T& lower, const T& upper) {
    return counter(lower)
        | op::take_while([upper] (const auto& value) {
            return value <= upper;
        });
}

template<typename T, typename U>
auto closed_range(const T& lower, const T& upper, const U& increment) {
    return counter(lower, increment)
        | op::take_while([upper] (const auto& value) {
            return value <= upper;
        });
}

template<typename T,
         template<typename> class Distribution,
         typename Engine = std::default_random_engine,
         typename Seed,
         typename... GenArgs>
auto randoms_seeded(Seed&& seed, GenArgs&&... args) {
    return generate(detail::RandomGenerator<Distribution, Engine, T>
        (seed, std::forward<GenArgs>(args)...));
}

template<typename T,
         template<typename> class Distribution,
         typename Engine = std::default_random_engine,
         typename... GenArgs>
auto randoms(GenArgs&&... args) {
    return randoms_seeded<T, Distribution, Engine>
        (detail::default_seed(), std::forward<GenArgs>(args)...);
}

template<typename Engine = std::default_random_engine, typename T, typename Seed>
auto uniform_random_ints(T lower, T upper, Seed&& seed) {
    return randoms_seeded<T, std::uniform_int_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename Engine = std::default_random_engine, typename T>
auto uniform_random_ints(T lower, T upper) {
    return uniform_random_ints<Engine, T>(lower, upper, detail::default_seed());
}

template<typename Engine = std::default_random_engine, typename T, typename Seed>
auto uniform_random_reals(T lower, T upper, Seed&& seed) {
    return randoms_seeded<T, std::uniform_real_distribution, Engine, Seed>
        (std::forward<Seed>(seed), lower, upper);
}

template<typename Engine = std::default_random_engine, typename T = double>
auto uniform_random_reals(T lower = 0.0, T upper = 1.0) {
    return uniform_random_reals<Engine, T>(lower, upper, detail::default_seed());
}

template<typename Engine = std::default_random_engine, typename T, typename Seed>
auto normal_randoms(T mean, T stddev, Seed&& seed) {
    return randoms_seeded<T, std::normal_distribution, Engine, Seed>
        (std::forward<Seed>(seed), mean, stddev);
}

template<typename Engine = std::default_random_engine, typename T = double>
auto normal_randoms(T mean = 0.0, T stddev = 1.0) {
    return normal_randoms<Engine, T>(mean, stddev, detail::default_seed());
}

template<typename Engine = std::default_random_engine, typename T = bool, typename Seed>
auto coin_flips(Seed&& seed) {
    return uniform_random_ints<Engine, T>(0, 1, std::forward<Seed>(seed));
}

template<typename Engine = std::default_random_engine, typename T = bool>
auto coin_flips() {
    return uniform_random_ints<Engine, T>(0, 1);
}

template<typename T>
auto singleton(T&& value) {
    using R = RemoveRef<T>;
    return provider::make_stream<provider::singleton<R>>(std::forward<T>(value));
}

template<typename T>
auto from(std::initializer_list<T> init) {
    return cycle(init, 1);
}

template<typename Container>
auto from_move(Container&& cont) {
    using T = ContainerType<Container>;
    return cycle_move(std::forward<Container>(cont), 1);
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

} /* namespace make_stream */
} /* namespace stream */

#endif
