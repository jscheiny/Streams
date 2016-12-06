#ifndef SCHEINERMAN_STREAM_STREAM_TERMINATORS_H
#define SCHEINERMAN_STREAM_STREAM_TERMINATORS_H

namespace stream {
namespace op {

#define CLASS_SPECIALIZATIONS(operation) \
    template<typename R, typename C> auto operation (R (C::*member)()) \
        { return operation (std::mem_fn(member)); } \
    template<typename R, typename C> auto operation (R (C::*member)() const) \
        { return operation (std::mem_fn(member)); }

template<typename Function>
inline auto for_each(Function&& function) {
    return make_terminator("stream::op::for_each", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        while(source->advance()) {
            function(std::move(*source->get()));
        }
        return function;
    });
}

CLASS_SPECIALIZATIONS(for_each);

inline auto count() {
    return make_terminator("stream::op::count", [=](auto&& stream) {
        auto& source = stream.getSource();
        size_t count = 0;
        while(source->advance()) {
            std::move(*source->get());
            count++;
        }
        return count;
    });
}

template<typename U, typename Accumulator>
inline auto identity_reduce(const U& identity, Accumulator&& accumulator) {
    return make_terminator("stream::op::identity_reduce", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        U result = identity;
        while(source->advance()) {
            result = accumulator(std::move(result), std::move(*source->get()));
        }
        return result;
    });
}

template<typename Accumulator>
inline auto reduce(Accumulator&& accumulator) {
    return make_terminator("stream::op::reduce", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        if(source->advance()) {
            auto reduction = identity_reduce(std::move(*source->get()),
                                             std::forward<Accumulator>(accumulator));
            return stream | reduction;
        } else {
            throw EmptyStreamException("stream::op::reduce");
        }
    });
}

template<typename IdentityFn, typename Accumulator>
inline auto reduce(IdentityFn&& identityFn, Accumulator&& accumulator) {
    return make_terminator("stream::op::reduce", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        if(source->advance()) {
            auto reduction = identity_reduce(identityFn(std::move(*source->get())),
                                             std::forward<Accumulator>(accumulator));
            return stream | reduction;
        } else {
            throw EmptyStreamException("stream::op::reduce");
        }
    });
}

inline auto first() {
    return make_terminator("stream::op::first", [=](auto&& stream) {
        auto& source = stream.getSource();
        if(source->advance()) {
            return std::move(*source->get());
        } else {
            throw EmptyStreamException("stream::op::first");
        }
    });
}

inline auto last() {
    return reduce([](auto&& first, auto&& second) { return second; })
        .rename("stream::op::last");
}

inline auto nth(size_t index) {
    return (skip(index) | first()).rename("stream::op::nth");
}

inline auto sum() {
    return reduce(std::plus<void>())
        .rename("stream::op::sum");
}

template<typename T>
inline auto sum(const T& identity) {
    return identity_reduce(identity, std::plus<T>())
        .rename("stream::op::sum");
}

inline auto product() {
    return reduce(std::multiplies<void>())
        .rename("stream::op::product");
}

template<typename T>
inline auto product(const T& identity) {
    return identity_reduce(identity, std::multiplies<T>())
        .rename("stream::op::product");
}

template<typename Less = std::less<void>>
inline auto max(Less&& less = Less()) {
    return reduce([=](const auto& a, const auto& b) {
        return less(a, b) ? b : a;
    }).rename("stream::op::max");
}

template<typename Function, typename Less = std::less<void>>
inline auto max_by(Function&& function, Less&& less = Less()) {
    /* Pair is (max, max_elem) */
    auto to_pair = [function](auto&& x) { return std::make_pair(function(x), x); };
    auto next = [function, less](auto&& accumulated, auto&& value) {
        auto fvalue = function(std::move(value));
        if(less(accumulated.first, fvalue)) {
            return std::make_pair(std::move(fvalue), std::move(value));
        }
        return accumulated;
    };

    return reduce(to_pair, next)
        .then([](auto&& accumulated) { return accumulated.second; })
        .rename("stream::op::max_by");
}

template<typename Less = std::less<void>>
inline auto min(Less&& less = Less()) {
    return reduce([=](const auto& a, const auto& b) {
        return less(a, b) ? a : b;
    }).rename("stream::op::min");
}

template<typename Function, typename Less = std::less<void>>
inline auto min_by(Function&& function, Less&& less = Less()) {
    /* Pair is (min, min_elem) */
    auto to_pair = [function](auto&& x) { return std::make_pair(function(x), x); };
    auto next = [function, less](auto&& accumulated, auto&& value) {
        auto fvalue = function(std::move(value));
        if(less(fvalue, accumulated.first)) {
            return std::make_pair(std::move(fvalue), std::move(value));
        }
        return accumulated;
    };

    return reduce(to_pair, next)
        .then([](auto&& accumulated) { return accumulated.second; })
        .rename("stream::op::min_by");
}

template<typename Less = std::less<void>>
inline auto minmax(Less&& less = Less()) {
    auto to_pair = [](auto&& x) { return std::make_pair(x, x); };
    auto next_minmax = [less](auto&& accumulated, auto&& value) {
        using T = std::remove_reference_t<decltype(value)>;
        if(less(value, accumulated.first)) {
            return std::pair<T,T>(std::move(value), std::move(accumulated.second));
        }
        if(less(accumulated.second, value)) {
            return std::pair<T,T>(std::move(accumulated.first), std::move(value));
        }
        return accumulated;
    };

    return reduce(to_pair, next_minmax).rename("stream::op::minmax");
}

template<typename Function, typename Less = std::less<void>>
inline auto minmax_by(Function&& function, Less&& less = Less()) {
    /* Pair is ((min, min_elem), (max, max_elem)) */
    auto to_pair = [function](auto&& x) {
      auto fx = function(x);
      return std::make_pair(std::make_pair(fx, x), std::make_pair(fx, x));
    };
    auto next = [function, less](auto&& accumulated, auto&& value) {
        auto fvalue = function(std::move(value));
        auto min = accumulated.first;
        if(less(fvalue, min.first)) {
            min = std::make_pair(std::move(fvalue), std::move(value));
        }
        auto max = accumulated.second;
        if(less(max.first, fvalue)) {
            max = std::make_pair(std::move(fvalue), std::move(value));
        }
        return std::make_pair(min, max);
    };

    return reduce(to_pair, next)
        .then([](auto&& accumulated) { return std::make_pair(accumulated.first.second, accumulated.second.second); })
        .rename("stream::op::minmax_by");
}

template<typename Predicate>
inline auto any(Predicate&& predicate) {
    return make_terminator("stream::op::any", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        while(source->advance()) {
            if(predicate(*source->get())) {
                return true;
            }
        }
        return false;
    });
}

inline auto any() {
    return any([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(any);

template<typename Predicate>
inline auto all(Predicate&& predicate) {
    return make_terminator("stream::op::all", [=](auto&& stream) mutable {
        auto& source = stream.getSource();
        while(source->advance()) {
            if(!predicate(*source->get())) {
                return false;
            }
        }
        return true;
    });
}

inline auto all() {
    return all([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(all);

template<typename Predicate>
inline auto none(Predicate&& predicate) {
    return any(std::forward<Predicate>(predicate))
        .then([](bool x) { return !x; })
        .rename("stream::op::none");
}

inline auto none() {
    return none([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(none);

template<typename Predicate>
inline auto not_all(Predicate&& predicate) {
    return all(std::forward<Predicate>(predicate))
        .then([](bool x) { return !x; })
        .rename("stream::op::not_all");
}

inline auto not_all() {
    return not_all([](bool b){ return b; });
}

CLASS_SPECIALIZATIONS(not_all);

template<typename OutputIterator>
inline auto copy_to(OutputIterator out) {
    return make_terminator("stream::op::copy_to", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        auto& source = stream.getSource();
        while(source->advance()) {
            *out = *source->get();
            ++out;
        }
        return out;
    });
}

template<typename OutputIterator>
inline auto move_to(OutputIterator out) {
    return make_terminator("stream::op::move_to", [=](auto&& stream) mutable {
        using T = StreamType<decltype(stream)>;
        auto& source = stream.getSource();
        while(source->advance()) {
            *out = std::move(*source->get());
            ++out;
        }
        return out;
    });
}

inline auto print_to(std::ostream& os, const char* delimiter = " ") {
    return make_terminator("stream::op::print_to", [&os, delimiter](auto&& stream) -> std::ostream& {
        using T = StreamType<decltype(stream)>;
        stream | copy_to(std::ostream_iterator<T>(os, delimiter));
        return os;
    });
}

inline auto random_sample(size_t size) {
    return make_terminator("stream::op::random_sample", [=](auto&& stream) {
        using T = StreamType<decltype(stream)>;
        auto& source = stream.getSource();

        std::vector<T> results;
        for(int i = 0; i < size; i++) {
            if(source->advance()) {
                results.push_back(std::move(*source->get()));
            } else {
                return results;
            }
        }

        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        auto random_index = [&generator](int upper) {
            return std::uniform_int_distribution<int>(0, upper - 1)(generator);
        };

        size_t seen = size;
        while(source->advance()) {
            seen++;
            int index = random_index(seen);
            if(index < size) {
                results[index] = std::move(*source->get());
            } else {
                source->get();
            }
        }

        return results;
    });
}

inline auto random_element() {
    return random_sample(1)
        .then([](auto&& vec) {
            if(vec.empty()) {
                throw EmptyStreamException("stream::op::random_element");
            }
            return vec[0];
        })
        .rename("stream::op::element");
}

#undef CLASS_SPECIALIZATIONS

} /* namespace op */
} /* namespace stream */


#endif
