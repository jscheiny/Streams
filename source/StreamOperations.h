#ifndef SCHEINERMAN_STREAM_STREAM_OPERATIONS_H
#define SCHEINERMAN_STREAM_STREAM_OPERATIONS_H

namespace stream {

template<typename F, typename G>
class Compose {
public:
    Compose(F&& f, G&& g) : f_(std::forward<F>(f)), g_(std::forward<G>(g)) {}

    template<typename T>
    std::result_of_t<F(std::result_of_t<G(Stream<T>&&)>)> operator() (Stream<T>&& stream) {
        return f_(g_(std::forward<Stream<T>>(stream)));
    }

private:
    F f_;
    G g_;
};

template<typename F>
class Operator {
public:
    Operator(const std::string& name, F&& op) : name_(name), operator_(std::forward<F>(op)) {}
    Operator(F&& op) : operator_(std::forward<F>(op)) {}

    template<typename T>
    std::result_of_t<F(Stream<T>&&)> apply_to(Stream<T>&& stream)  {
        if(!name_.empty()) {
            stream.check_vacant(name_);
        }
        return operator_(std::forward<Stream<T>>(stream));
    }

    template<typename G>
    Operator<Compose<G, F>> operator| (Operator<G>&& right) {
        return {{std::move(right.operator_), std::move(operator_)}};
    }

    template<typename G>
    Terminator<Compose<G, F>> operator| (Terminator<G>&& right) {
        return {{std::move(right.terminator_), std::move(operator_)}};
    }

    Operator<F> rename(const std::string& name) && {
        name_ = name;
        return std::move(*this);
    }

    template<typename> friend class Operator;
private:
    std::string name_;
    F operator_;
};


template<typename F>
class Terminator {
public:
    Terminator(const std::string& name, F&& term) : name_(name), terminator_(term) {}
    Terminator(F&& term) : terminator_(term) {}

    template<typename T>
    auto apply_to(Stream<T>&& stream) -> std::result_of_t<F(Stream<T>&&)> {
        if(!name_.empty()) {
            stream.check_vacant(name_);
        }
        try {
            return terminator_(std::forward<Stream<T>>(stream));
        } catch(EmptyStreamException& e) {
            if(!name_.empty()) {
                throw EmptyStreamException(name_);
            } else {
                throw;
            }
        }
        stream.close();
    }

    Terminator<F> rename(const std::string& name) && {
        name_ = name;
        return std::move(*this);
    }

    template<typename G>
    Terminator<Compose<G, F>> then(G&& function) {
        return {Compose<G, F>(std::forward<G>(function), std::move(terminator_))};
    }

    template<typename> friend class Operator;
private:
    std::string name_;
    F terminator_;
};

template<typename Function>
Operator<Function> make_operator(const std::string& name, Function&& function) {
    return Operator<Function>(name, std::forward<Function>(function));
}

template<typename Function>
Terminator<Function> make_terminator(const std::string& name, Function&& function) {
    return Terminator<Function>(name, std::forward<Function>(function));
}

} /* namespace stream */

#endif
