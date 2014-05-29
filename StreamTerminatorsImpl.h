#ifndef STREAM_TERMINATORS_IMPL_H
#define STREAM_TERMINATORS_IMPL_H

template<typename T, bool I>
size_t StreamImpl<T, I>::count() {
    check_vacant("count");
    size_t count = 0;
    while(source_->advance()) {
        source_->get();
        count++;
    }
    return count;
}

template<typename T, bool I>
template<typename U, typename Accumulator>
U StreamImpl<T, I>::reduce(const U& identity, Accumulator&& accumulator) {
    check_vacant("reduce");
    U result = identity;
    while(source_->advance()) {
        result = accumulator(result, *source_->get());
    }
    return result;
}

template<typename T, bool I>
template<typename Identity, typename Accumulator>
ReturnType<Identity, T&> StreamImpl<T, I>::reduce(Identity&& identity,
                                                  Accumulator&& accumulator) {
    check_vacant("reduce");
    if(source_->advance()) {
        return reduce(identity(*source_->get()),
                      std::forward<Accumulator>(accumulator));
    } else {
        throw EmptyStreamException("reduce");
    }
}

template<typename T, bool I>
template<typename Accumulator>
T StreamImpl<T, I>::reduce(Accumulator&& accumulator) {
    check_vacant("reduce");
    if(source_->advance()) {
        return reduce(*source_->get(), std::forward<Accumulator>(accumulator));
    } else {
        throw EmptyStreamException("reduce");
    }
}

template<typename T, bool I>
template<typename U>
U StreamImpl<T, I>::reduce_by(const Reducer<T, U>& reducer) {
    check_vacant("reduce_by");
    if(source_->advance()) {
        U result = reducer.initial(*source_->get());
        while(source_->advance()) {
            result = reducer.accumulate(result, *source_->get());
        }
        return result;
    } else {
        throw EmptyStreamException("reduce_by");
    }
}

template<typename T, bool I>
template<typename Function>
T StreamImpl<T, I>::no_identity_reduction(const std::string& name,
                                          Function&& function) {
    check_vacant(name);
    try {
        return reduce(std::forward<Function>(function));
    } catch(EmptyStreamException& e) {
        throw EmptyStreamException(name);
    }
}

template<typename T, bool I>
template<typename Identity, typename Function>
ReturnType<Identity, T&> StreamImpl<T, I>::no_identity_reduction(
            const std::string& name,
            Identity&& identity,
            Function&& function) {
    check_vacant(name);
    try {
        return reduce(std::forward<Identity>(identity),
                      std::forward<Function>(function));
    } catch(EmptyStreamException& e) {
        throw EmptyStreamException(name);
    }
}

template<typename T, bool I>
T StreamImpl<T, I>::sum() {
    return no_identity_reduction("sum", std::plus<T>());
}

template<typename T, bool I>
T StreamImpl<T, I>::sum(const T& identity) {
    return reduce(identity, std::plus<T>());
}

template<typename T, bool I>
T StreamImpl<T, I>::product() {
    return no_identity_reduction("product", std::multiplies<T>());
}

template<typename T, bool I>
T StreamImpl<T, I>::product(const T& identity) {
    return reduce(identity, std::multiplies<T>());
}

template<typename T, bool I>
template<typename Compare>
T StreamImpl<T, I>::max(Compare&& compare) {
    return no_identity_reduction("max", [less=std::forward<Compare>(compare)]
        (T& a, T& b) {
            return less(a, b) ? b : a;
        });
}

template<typename T, bool I>
template<typename Compare>
T StreamImpl<T, I>::min(Compare&& compare) {
    return no_identity_reduction("min", [less=std::forward<Compare>(compare)]
        (T& a, T& b) {
            return less(a, b) ? a : b;
        });
}

template<typename T, bool I>
template<typename Compare>
std::pair<T, T> StreamImpl<T, I>::minmax(Compare&& compare) {
    return no_identity_reduction("minmax",
        [](T& value) { return std::make_pair(value, value); },
        [less=std::forward<Compare>(compare)] (const auto& accumulated, T& value) {
            if(less(value, accumulated.first)) {
                return std::make_pair(value, accumulated.second);
            }
            if(less(accumulated.second, value)) {
                return std::make_pair(accumulated.first, value);
            }
            return accumulated;
        });
}

template<typename T, bool I>
T StreamImpl<T, I>::first() {
    if(source_->advance()) {
        return *source_->get();
    } else {
        throw EmptyStreamException("first");
    }
}

template<typename T, bool I>
T StreamImpl<T, I>::last() {
    return no_identity_reduction("last",
        [](T& first, T& second) { return second; });
}

template<typename T, bool I>
std::vector<T> StreamImpl<T, I>::random_sample(size_t size) {
    check_vacant("random_sample");

    std::vector<T> results;
    for(int i = 0; i < size; i++) {
        if(source_->advance()) {
            results.push_back(std::move(*source_->get()));
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
    while(source_->advance()) {
        seen++;
        int index = random_index(seen);
        if(index < size) {
            results[index] = std::move(*source_->get());
        } else {
            source_->get();
        }
    }

    return results;
}

template<typename T, bool I>
T StreamImpl<T, I>::random_element() {
    check_vacant("random_element");
    auto vec = random_sample(1);
    if(vec.empty()) {
        throw EmptyStreamException("random_element");
    }
    return vec[0];
}

template<typename T, bool I>
template<typename Predicate>
bool StreamImpl<T, I>::any(Predicate&& predicate) {
    check_vacant("any");
    while(source_->advance()) {
        if(predicate(*source_->get())) {
            return true;
        }
    }
    return false;
}

template<typename T, bool I>
std::enable_if_t<StreamImpl<T, I>::is_boolable, bool> StreamImpl<T, I>::any() {
    return any(to_bool);
}

template<typename T, bool I>
template<typename Predicate>
bool StreamImpl<T, I>::all(Predicate&& predicate) {
    check_vacant("all");
    while(source_->advance()) {
        if(!predicate(*source_->get())) {
            return false;
        }
    }
    return true;
}

template<typename T, bool I>
std::enable_if_t<StreamImpl<T, I>::is_boolable, bool> StreamImpl<T, I>::all() {
    return all(to_bool);
}

template<typename T, bool I>
template<typename Predicate>
bool StreamImpl<T, I>::none(Predicate&& predicate) {
    check_vacant("none");
    return !any(std::forward<Predicate>(predicate));
}

template<typename T, bool I>
std::enable_if_t<StreamImpl<T, I>::is_boolable, bool> StreamImpl<T, I>::none() {
    return none(to_bool);
}

template<typename T, bool I>
std::enable_if_t<StreamImpl<T, I>::is_boolable, bool> StreamImpl<T, I>::not_all() {
    check_vacant("not_all");
    return !all(to_bool);
}


template<typename T, bool I>
template<typename OutputIterator>
void StreamImpl<T, I>::copy_to(OutputIterator out) {
    check_vacant("copy_to");
    while(source_->advance()) {
        *out = *source_->get();
        out++;
    }
}

template<typename T, bool I>
template<typename OutputIterator>
void StreamImpl<T, I>::move_to(OutputIterator out) {
    check_vacant("move_to");
    while(source_->advance()) {
        *out = std::move(*source_->get());
        out++;
    }
}

template<typename T, bool I>
void StreamImpl<T, I>::print_to(std::ostream& os, const char* delimiter) {
    check_vacant("print_to");
    copy_to(std::ostream_iterator<T>(os, delimiter));
}


template<typename T, bool I>
std::vector<T> StreamImpl<T, I>::to_vector() {
    check_vacant("to_vector");
    std::vector<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T, bool I>
std::list<T> StreamImpl<T, I>::to_list() {
    check_vacant("to_list");
    std::list<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T, bool I>
std::deque<T> StreamImpl<T, I>::to_deque() {
    check_vacant("to_deque");
    std::deque<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T, bool I>
template<typename Compare>
std::set<T, Compare> StreamImpl<T, I>::to_set(Compare&& compare) {
    check_vacant("to_set");
    std::set<T, Compare> result;
    copy_to(inserter(result, result.end()));
    return result;
}

template<typename T, bool I>
template<typename Compare>
std::multiset<T, Compare> StreamImpl<T, I>::to_multiset(Compare&& compare) {
    check_vacant("to_multiset");
    std::multiset<T, Compare> result;
    copy_to(inserter(result, result.end()));
    return result;
}

template<typename T, bool I>
template<typename Function>
void StreamImpl<T, I>::for_each(Function&& function) {
    check_vacant("for_each");
    while(source_->advance()) {
        function(*source_->get());
    }
}

#endif