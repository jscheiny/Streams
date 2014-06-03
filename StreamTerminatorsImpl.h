#ifndef STREAM_TERMINATORS_IMPL_H
#define STREAM_TERMINATORS_IMPL_H

template<typename T>
size_t StreamImpl<T, Common>::count() {
    check_vacant("count");
    size_t count = 0;
    while(source_->advance()) {
        source_->get();
        count++;
    }
    return count;
}

template<typename T>
template<typename U, typename Accumulator>
U StreamImpl<T, Common>::reduce(const U& identity, Accumulator&& accumulator) {
    check_vacant("reduce");
    U result = identity;
    while(source_->advance()) {
        result = accumulator(std::move(result), std::move(*source_->get()));
    }
    return result;
}

template<typename T>
template<typename Identity, typename Accumulator>
ReturnType<Identity, T&&> StreamImpl<T, Common>::reduce(Identity&& identity,
                                                  Accumulator&& accumulator) {
    check_vacant("reduce");
    if(source_->advance()) {
        return reduce(identity(std::move(*source_->get())),
                      std::forward<Accumulator>(accumulator));
    } else {
        throw EmptyStreamException("reduce");
    }
}

template<typename T>
template<typename Accumulator>
T StreamImpl<T, Common>::reduce(Accumulator&& accumulator) {
    check_vacant("reduce");
    if(source_->advance()) {
        return reduce(std::move(*source_->get()), std::forward<Accumulator>(accumulator));
    } else {
        throw EmptyStreamException("reduce");
    }
}

template<typename T>
template<typename U>
U StreamImpl<T, Common>::reduce_by(const Reducer<T, U>& reducer) {
    check_vacant("reduce_by");
    if(source_->advance()) {
        U result = reducer.initial(std::move(*source_->get()));
        while(source_->advance()) {
            result = reducer.accumulate(std::move(result), std::move(*source_->get()));
        }
        return result;
    } else {
        throw EmptyStreamException("reduce_by");
    }
}

template<typename T>
template<typename Function>
T StreamImpl<T, Common>::no_identity_reduction(const std::string& name,
                                               Function&& function) {
    check_vacant(name);
    try {
        return reduce(std::forward<Function>(function));
    } catch(EmptyStreamException& e) {
        throw EmptyStreamException(name);
    }
}

template<typename T>
template<typename Identity, typename Function>
ReturnType<Identity, T&> StreamImpl<T, Common>::no_identity_reduction(
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

template<typename T>
T StreamImpl<T, Common>::sum() {
    return no_identity_reduction("sum", std::plus<T>());
}

template<typename T>
T StreamImpl<T, Common>::sum(const T& identity) {
    return reduce(identity, std::plus<T>());
}

template<typename T>
T StreamImpl<T, Common>::product() {
    return no_identity_reduction("product", std::multiplies<T>());
}

template<typename T>
T StreamImpl<T, Common>::product(const T& identity) {
    return reduce(identity, std::multiplies<T>());
}

template<typename T>
template<typename Compare>
T StreamImpl<T, Common>::max(Compare&& compare) {
    return no_identity_reduction("max", [less=std::forward<Compare>(compare)]
        (T& a, T& b) {
            return less(a, b) ? b : a;
        });
}

template<typename T>
template<typename Compare>
T StreamImpl<T, Common>::min(Compare&& compare) {
    return no_identity_reduction("min", [less=std::forward<Compare>(compare)]
        (T& a, T& b) {
            return less(a, b) ? a : b;
        });
}

template<typename T>
template<typename Compare>
std::pair<T, T> StreamImpl<T, Common>::minmax(Compare&& compare) {
    return no_identity_reduction("minmax",
        [](T value) { return std::make_pair(value, value); },
        [less=std::forward<Compare>(compare)] (auto&& accumulated, T&& value) {
            if(less(value, accumulated.first)) {
                return std::pair<T,T>(std::forward<T>(value), accumulated.second);
            }
            if(less(accumulated.second, value)) {
                return std::pair<T,T>(accumulated.first, std::forward<T>(value));
            }
            return accumulated;
        });
}

template<typename T>
T StreamImpl<T, Common>::first() {
    check_vacant("first");
    if(source_->advance()) {
        return std::move(*source_->get());
    } else {
        throw EmptyStreamException("first");
    }
}

template<typename T>
T StreamImpl<T, Common>::last() {
    return no_identity_reduction("last",
        [](T& first, T& second) { return second; });
}

template<typename T>
T StreamImpl<T, Common>::nth(size_t index) {
    check_vacant("nth");
    try {
        return skip(index).first();
    } catch(EmptyStreamException& e) {
        throw EmptyStreamException("nth");
    }
}

template<typename T>
T StreamImpl<T, Common>::operator[] (size_t index) {
    check_vacant("operator[]");
    try {
        return nth(index);
    } catch(EmptyStreamException& e) {
        throw EmptyStreamException("operator[]");
    }
}

template<typename T>
std::vector<T> StreamImpl<T, Common>::random_sample(size_t size) {
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

template<typename T>
T StreamImpl<T, Common>::random_element() {
    check_vacant("random_element");
    auto vec = random_sample(1);
    if(vec.empty()) {
        throw EmptyStreamException("random_element");
    }
    return vec[0];
}

template<typename T>
template<typename Predicate>
bool StreamImpl<T, Common>::any(Predicate&& predicate) {
    check_vacant("any");
    while(source_->advance()) {
        if(predicate(*source_->get())) {
            return true;
        }
    }
    return false;
}

template<typename T>
template<typename Predicate>
bool StreamImpl<T, Common>::all(Predicate&& predicate) {
    check_vacant("all");
    while(source_->advance()) {
        if(!predicate(*source_->get())) {
            return false;
        }
    }
    return true;
}

template<typename T>
template<typename Predicate>
bool StreamImpl<T, Common>::none(Predicate&& predicate) {
    check_vacant("none");
    return !any(std::forward<Predicate>(predicate));
}

template<typename T>
template<typename OutputIterator>
OutputIterator StreamImpl<T, Common>::copy_to(OutputIterator out) {
    check_vacant("copy_to");
    while(source_->advance()) {
        *out = std::move(*source_->get());
        out++;
    }
    return out;
}

template<typename T>
template<typename OutputIterator>
OutputIterator StreamImpl<T, Common>::move_to(OutputIterator out) {
    check_vacant("move_to");
    while(source_->advance()) {
        *out = std::move(*source_->get());
        out++;
    }
    return out;
}

template<typename T>
void StreamImpl<T, Common>::print_to(std::ostream& os, const char* delimiter) {
    check_vacant("print_to");
    copy_to(std::ostream_iterator<T>(os, delimiter));
}


template<typename T>
std::vector<T> StreamImpl<T, Common>::to_vector() {
    check_vacant("to_vector");
    std::vector<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
std::list<T> StreamImpl<T, Common>::to_list() {
    check_vacant("to_list");
    std::list<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
std::deque<T> StreamImpl<T, Common>::to_deque() {
    check_vacant("to_deque");
    std::deque<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T>
template<typename Compare>
std::set<T, Compare> StreamImpl<T, Common>::to_set(Compare&& compare) {
    check_vacant("to_set");
    std::set<T, Compare> result;
    copy_to(inserter(result, result.end()));
    return result;
}

template<typename T>
template<typename Compare>
std::multiset<T, Compare> StreamImpl<T, Common>::to_multiset(Compare&& compare) {
    check_vacant("to_multiset");
    std::multiset<T, Compare> result;
    copy_to(inserter(result, result.end()));
    return result;
}

template<typename T>
template<typename Function>
void StreamImpl<T, Common>::for_each(Function&& function) {
    check_vacant("for_each");
    while(source_->advance()) {
        function(std::move(*source_->get()));
    }
}

#endif