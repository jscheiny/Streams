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

template<typename Provider> class Operator;
template<typename Provider> class Terminator;

template<typename Provider>
class Stream {

public:
    using provider_t = Provider;
    using element = typename Provider::element;
    using T = element;

    static_assert(!std::is_void<typename Provider::element>::value,
        "Element type of stream cannot be void");

    static_assert(provider::detail::has_get<Provider, std::shared_ptr<element>>::value,
        "Stream provider must have a method std::shared_ptr<T> get().");
    static_assert(provider::detail::has_advance<Provider, bool>::value,
        "Stream provider must have a method bool advance().");
    static_assert(provider::detail::has_print<Provider, provider::print_info, std::ostream&, int>::value,
        "Stream provider must have a method print_info print(std::ostream&, int) const.");

    auto begin() {
        return provider::begin(source_);
    }

    auto end() {
        return provider::end(source_);
    }

    Stream() : source_(nullptr) {}

    Stream(std::unique_ptr<Provider> source)
        : source_(std::move(source)) {}

    Stream(Stream<Provider>&& other) = default;
    Stream<Provider>& operator= (Stream<Provider>&& other) = default;

    Stream(const Stream<Provider>& other) = delete;
    Stream<Provider>& operator= (const Stream<Provider>& other) = default;

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

    std::unique_ptr<Provider>& source() {
        return source_;
    }

    void close() {
        source_.reset();
    }

    bool occupied() const {
        return bool(source_);
    }

    void swap(Stream<Provider>& other) {
        source_.swap(other.source_);
    }

    std::string pipeline() {
        std::stringstream ss;
        auto info = source_->print(ss, 1);
        ss << "Stream pipeline with "
           << info.stages << " stage" << (info.stages == 1 ? "" : "s") << " and "
           << info.sources << " source" << (info.sources == 1 ? "" : "s") << ".";
        return ss.str();
    }

    template<typename> friend class Operator;
    template<typename> friend class Terminator;

private:
    std::unique_ptr<Provider> source_;

    void check_vacant(const std::string& method) {
        if (!occupied()) {
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
