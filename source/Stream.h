#ifndef SCHEINERMAN_STREAM_STREAM_H
#define SCHEINERMAN_STREAM_STREAM_H

#include "StreamForward.h"
#include "StreamError.h"
#include "providers/Providers.h"
#include "Utility.h"

#include <unordered_set>
#include <functional>
#include <type_traits>
#include <iostream>
#include <iterator>
#include <vector>
#include <random>
#include <chrono>
#include <deque>
#include <list>
#include <set>

namespace stream {

struct MakeStream {
    template<typename T>
    static Stream<RemoveRef<T>> empty();

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value);

    template<typename T>
    static Stream<RemoveRef<T>> repeat(T&& value, size_t times);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> cycle(Iterator begin, Iterator end,
                                                size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle(const Container& cont,
                                                  size_t times);

    template<typename T>
    static Stream<T> cycle(std::initializer_list<T> init);

    template<typename T>
    static Stream<T> cycle(std::initializer_list<T> init, size_t times);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle_move(Container&& cont);

    template<typename Container>
    static Stream<ContainerType<Container>> cycle_move(Container&& cont,
                                                       size_t times);

    template<typename Generator>
    static Stream<std::result_of_t<Generator()>> generate(Generator&& generator);

    template<typename T, typename Function>
    static Stream<RemoveRef<T>> iterate(T&& initial, Function&& function);

    template<typename... Args, typename Function>
    static Stream<RemoveRef<Head<Args...>>> recurrence(Function&& function, Args&&... initial);

    template<typename T>
    static Stream<RemoveRef<T>> counter(T&& start);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> counter(T&& start, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> counter(T&& start, const U& increment);

    template<typename T>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> range(T&& lower, T&& upper, const U& increment);

    template<typename T>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper, U&& increment);

    template<typename T, typename U>
    static Stream<RemoveRef<T>> closed_range(T&& lower, T&& upper, const U& increment);

    template<typename T, template<typename> class Distribution,
             typename Engine=std::default_random_engine,
             typename Seed,
             typename ... GenArgs>
    static Stream<T> randoms_seeded(Seed&& seed, GenArgs&&... args);

    template<typename T, template<typename> class Distribution,
             typename Engine=std::default_random_engine,
             typename ... GenArgs>
    static Stream<T> randoms(GenArgs&&... args);

    template<typename Engine=std::default_random_engine, typename T>
    static Stream<T> uniform_random_ints(T lower, T upper);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> uniform_random_ints(T lower, T upper, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=double>
    static Stream<T> uniform_random_reals(T lower=0.0, T upper=1.0);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> uniform_random_reals(T lower, T upper, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=double>
    static Stream<T> normal_randoms(T mean=0.0, T stddev=1.0);

    template<typename Engine=std::default_random_engine, typename T, typename Seed>
    static Stream<T> normal_randoms(T mean, T stddev, Seed&& seed);

    template<typename Engine=std::default_random_engine, typename T=bool>
    static Stream<T> coin_flips();

    template<typename Engine=std::default_random_engine, typename T=bool, typename Seed>
    static Stream<T> coin_flips(Seed&& seed);

    template<typename T>
    static Stream<RemoveRef<T>> singleton(T&& value);

    template<typename Iterator>
    static Stream<IteratorType<Iterator>> from(Iterator begin, Iterator end);

    template<typename Container>
    static Stream<ContainerType<Container>> from(const Container& cont);

    template<typename T>
    static Stream<T> from(T* array, std::size_t length);

    template<typename T>
    static Stream<T> from(std::initializer_list<T> init);

    template<typename Container>
    static Stream<ContainerType<Container>> from_move(Container&& cont);

private:
    static auto default_seed() {
        return std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }

};

template<typename T> class Operator;
template<typename T> class Terminator;

template<typename T>
class Stream {

public:
    using element_type = T;
    using iterator = typename provider::StreamProvider<T>::Iterator;

    iterator begin() {
        return source_->begin();
    }

    iterator end() {
        return source_->end();
    }

    Stream()
        : source_(make_stream_provider<provider::Empty, T>()) {}

    Stream(Stream<T>&& other) = default;
    Stream<T>& operator= (Stream<T>&& other) = default;

    Stream(const Stream<T>& other) = delete;
    Stream<T>& operator= (const Stream<T>& other) = default;

    Stream(StreamProviderPtr<T> source)
        : source_(std::move(source)) {}

    template<typename Iterator>
    Stream(Iterator begin, Iterator end)
        : source_(make_stream_provider<provider::Iterator, T, Iterator>(
            begin, end)) {}

    template<typename Container>
    Stream(const Container& container)
        : Stream(container.begin(), container.end()) {}

    Stream(std::initializer_list<T> init)
        : Stream(std::deque<T>(init.begin(), init.end())) {}

    template<typename F>
    auto operator| (Operator<F>&& op) ->
            decltype(op.apply_to(std::move(*this))) {
        return op.apply_to(std::move(*this));
    }

    template<typename F>
    auto operator| (Operator<F>& op) ->
            decltype(op.apply_to(std::move(*this))) {
        return op.apply_to(std::move(*this));
    }

    template<typename F>
    auto operator| (Terminator<F>&& term) ->
            decltype(term.apply_to(std::move(*this))) {
        return term.apply_to(std::move(*this));
    }

    template<typename F>
    auto operator| (Terminator<F>& term) ->
            decltype(term.apply_to(std::move(*this))) {
        return term.apply_to(std::move(*this));
    }

    template<typename A> operator std::vector<T, A>();
    template<typename A> operator std::list<T, A>();
    template<typename A> operator std::deque<T, A>();
    template<typename C, typename A> operator std::set<T, C, A>();
    template<typename C, typename A> operator std::multiset<T, C, A>();
    template<typename H, typename P, typename A> operator std::unordered_set<T, H, P, A>();
    template<typename H, typename P, typename A> operator std::unordered_multiset<T, H, P, A>();

    StreamProviderPtr<T>& getSource() {
        return source_;
    }

    void close() {
        source_.reset();
    }

    bool occupied() const {
        return bool(source_);
    }

    void swap(Stream<T>& other) {
        source_.swap(other.sorce_);
    }

    std::string pipeline() {
        std::stringstream ss;
        provider::PrintInfo info = source_->print(ss, 1);
        ss << "Stream pipeline with "
           << info.stages << " stage" << (info.stages == 1 ? "" : "s") << " and "
           << info.sources << " source" << (info.sources == 1 ? "" : "s") << ".";
        return ss.str();
    }

    template<typename> friend class Operator;
    template<typename> friend class Terminator;

private:
    StreamProviderPtr<T> source_;

    void check_vacant(const std::string& method) {
        if(!occupied()) {
            throw VacantStreamException(method);
        }
    }

};

} /* namespace stream */

#include "StreamOperations.h"
#include "StreamOperators.h"
#include "StreamTerminators.h"
#include "StreamGenerators.h"
#include "StreamAlgebra.h"
#include "StreamConversions.h"

#endif
