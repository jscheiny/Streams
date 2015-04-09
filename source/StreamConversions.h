#ifndef SCHEINERMAN_STREAM_STREAM_CONVERSIONS_H
#define SCHEINERMAN_STREAM_STREAM_CONVERSIONS_H

namespace stream {

namespace op {

namespace detail {

template<template<typename T, typename A> class ListContainer>
class ListContainerTerminatorMaker {

public:
    ListContainerTerminatorMaker(const std::string& name) : name_(name) {}

    auto operator() () const {
        return make_terminator(name_, [](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            ListContainer<T, std::allocator<T>> result;
            stream | copy_to(std::back_inserter(result));
            return result;
        });
    }

    template<typename Allocator = std::allocator<void>>
    auto operator() (const Allocator& allocator) const {
        return make_terminator(name_, [allocator](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            ListContainer<T, Allocator> result(allocator);
            stream | copy_to(std::back_inserter(result));
            return result;
        });
    }

private:
    std::string name_;
};

template<template<typename T, typename C, typename A> class OrderedContainer>
class OrderedContainerTerminatorMaker {

public:
    OrderedContainerTerminatorMaker(const std::string& name) : name_(name) {}

    template<typename Compare, typename Allocator>
    auto operator() (const Compare& compare,
                     const Allocator& allocator) const {
        return make_terminator(name_, [compare, allocator](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            OrderedContainer<T, Compare, Allocator> result(compare, allocator);
            stream | copy_to(std::inserter(result, result.end()));
            return result;
        });
    }

    template<typename Compare = std::less<void>>
    auto operator() (const Compare& compare = Compare()) const {
        return make_terminator(name_, [compare](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            OrderedContainer<T, Compare, std::allocator<T>> result(compare);
            stream | copy_to(std::inserter(result, result.end()));
            return result;
        });
    }

private:
    std::string name_;
};

struct PolymorphicHash {
    template<typename T>
    decltype(auto) operator() (const T& value) {
        return std::hash<T>{}(value);
    }
};

template<template<typename T, typename H, typename P, typename A> class UnorderedContainer>
class UnorderedContainerTerminatorMaker {

public:
    UnorderedContainerTerminatorMaker(const std::string& name) : name_(name) {}

    template<typename Hash, typename Predicate, typename Allocator>
    auto operator() (const Hash& hash,
                     const Predicate& predicate,
                     const Allocator& allocator) const {
        return make_terminator(name_, [hash, predicate, allocator](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            UnorderedContainer<T, Hash, Predicate, Allocator> result(0, hash, predicate, allocator);
            stream | copy_to(std::inserter(result, result.end()));
            return result;
        });
    }

    template<typename Hash = PolymorphicHash,
             typename Predicate = std::equal_to<void>>
    auto operator() (const Hash& hash = Hash(),
                     const Predicate& predicate = Predicate()) const {
        return make_terminator(name_, [hash, predicate](auto&& stream) {
            using T = StreamType<decltype(stream)>;
            UnorderedContainer<T, Hash, Predicate, std::allocator<T>> result(0, hash, predicate);
            stream | copy_to(std::inserter(result, result.end()));
            return result;
        });
    }

private:
    std::string name_;
};

} /* namespace detail */

static const detail::ListContainerTerminatorMaker<std::vector> to_vector{"stream::op::to_vector"};
static const detail::ListContainerTerminatorMaker<std::list> to_list{"stream::op::to_list"};
static const detail::ListContainerTerminatorMaker<std::deque> to_deque{"stream::op::to_deque"};
static const detail::OrderedContainerTerminatorMaker<std::set> to_set{"stream::op::to_set"};
static const detail::OrderedContainerTerminatorMaker<std::multiset> to_multiset{"stream::op::to_multiset"};
static const detail::UnorderedContainerTerminatorMaker<std::unordered_set> to_unordered_set{"stream::op::to_unordered_set"};
static const detail::UnorderedContainerTerminatorMaker<std::unordered_multiset> to_unordered_multiset{"stream::op::to_unordered_multiset"};

} /* namespace op */

template<typename T>
template<typename Allocator>
Stream<T>::operator std::vector<T, Allocator>() {
    return (*this) | op::to_vector(Allocator());
}

template<typename T>
template<typename Allocator>
Stream<T>::operator std::list<T, Allocator>() {
    return (*this) | op::to_list(Allocator());
}

template<typename T>
template<typename Allocator>
Stream<T>::operator std::deque<T, Allocator>() {
    return (*this) | op::to_deque(Allocator());
}

template<typename T>
template<typename Compare, typename Allocator>
Stream<T>::operator std::set<T, Compare, Allocator>() {
    return (*this) | op::to_set(Compare(), Allocator());
}

template<typename T>
template<typename Compare, typename Allocator>
Stream<T>::operator std::multiset<T, Compare, Allocator>() {
    return (*this) | op::to_multiset(Compare(), Allocator());
}

template<typename T>
template<typename Hash, typename Predicate, typename Allocator>
Stream<T>::operator std::unordered_set<T, Hash, Predicate, Allocator>() {
    return (*this) | op::to_unordered_set(Hash(), Predicate(), Allocator());
}

template<typename T>
template<typename Hash, typename Predicate, typename Allocator>
Stream<T>::operator std::unordered_multiset<T, Hash, Predicate, Allocator>() {
    return (*this) | op::to_unordered_multiset(Hash(), Predicate(), Allocator());
}

} /* namespace stream */

#endif
