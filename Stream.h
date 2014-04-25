#ifndef STREAM_H
#define STREAM_H

#include "Providers.h"
#include "Utility.h"

#include <functional>
#include <type_traits>
#include <iterator>
#include <vector>
#include <list>

template<typename T, template<typename> class P> class BasicStream;

template<typename T> using UniquePtr = std::unique_ptr<T>;
template<typename T> using SharedPtr = std::shared_ptr<T>;
template<typename T> using UniqueStream = BasicStream<T, UniquePtr>;
template<typename T> using SharedStream = BasicStream<T, SharedPtr>;
template<typename T> using Stream = UniqueStream<T>;

template<typename T, template<typename> class Pointer>
class BasicStream {

public:
    template<typename Generator>
    static BasicStream<T, Pointer> generate(Generator&& generator);

    template<typename Function>
    static BasicStream<T, Pointer> iterate(T initial, Function&& function);

    template<typename Iterator>
    BasicStream(Iterator begin, Iterator end)
        : source_(make_unique<IteratorStreamProvider<T, Pointer, Iterator>>(
            begin, end)) {}

    template<typename Predicate>
    BasicStream<T, Pointer> filter(Predicate&& predicate);

    template<typename Predicate>
    BasicStream<T, Pointer> take_while(Predicate&& predicate);

    template<typename Predicate>
    BasicStream<T, Pointer> take_until(Predicate&& predicate);

    template<typename Predicate>
    BasicStream<T, Pointer> drop_while(Predicate&& predicate);

    template<typename Predicate>
    BasicStream<T, Pointer> drop_until(Predicate&& predicate);

    template<typename Action>
    BasicStream<T, Pointer> peek(Action&& action);

    template<typename Transform>
    BasicStream<ReturnType<Transform, T>, Pointer> map(Transform&& transform);

    BasicStream<T, Pointer> limit(std::size_t length);

    BasicStream<T, Pointer> skip(std::size_t amount);

    template<typename Compare = std::less<T>>
    BasicStream<T, Pointer> sort(Compare&& comparator = Compare());

    template<typename Compare = std::less<T>>
    BasicStream<T, Pointer> distinct(Compare&& comparator = Compare());

    template<typename Iterator>
    BasicStream<T, Pointer> extend(Iterator begin, Iterator end);

    BasicStream<T, Pointer> extend(BasicStream<T, Pointer>&& other);

    template<typename Other>
    BasicStream<ZipResult<T, Other>, Pointer>
    zip(BasicStream<Other, Pointer>&& other);

    size_t count();

    template<typename OutputIterator>
    void copy_to(OutputIterator out);

    std::vector<T> as_vector();

    std::list<T> as_list();

    template<typename Function>
    void for_each(Function&& function);

    bool occupied() const {
        return source_;
    }

    template<typename X, template<typename> class P> friend class BasicStream;

private:
    BasicStream(StreamProviderPtr<T, Pointer> source)
        : source_(std::move(source)) {}

    StreamProviderPtr<T, Pointer> source_;

};

template<typename T, template<typename> class P>
template<typename Generator>
BasicStream<T, P> BasicStream<T, P>::generate(Generator&& generator) {

    auto source = StreamProviderPtr<T, P>(
        new GeneratedStreamProvider<T, P, Generator>(
            std::forward<Generator>(generator)));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Function>
BasicStream<T, P> BasicStream<T, P>::iterate(T initial, Function&& function) {

    auto source = StreamProviderPtr<T, P>(
        new IteratedStreamProvider<T, P, Function>(
            std::forward<T>(initial), std::forward<Function>(function)));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::filter(Predicate&& predicate) {

    auto new_source = StreamProviderPtr<T, P>(
        new FilteredStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::take_while(Predicate&& predicate) {

    auto source = StreamProviderPtr<T, P>(
        new TakeWhileStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate),
            false));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::take_until(Predicate&& predicate) {

    auto source = StreamProviderPtr<T, P>(
        new TakeWhileStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate), true));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::drop_while(Predicate&& predicate) {

    auto source = StreamProviderPtr<T, P>(
        new DropWhileStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate), false));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::drop_until(Predicate&& predicate) {

    auto source = StreamProviderPtr<T, P>(
        new DropWhileStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate), true));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Action>
BasicStream<T, P> BasicStream<T, P>::peek(Action&& action) {

    auto new_source = StreamProviderPtr<T, P>(
        new PeekStreamProvider<T, P, Action>(
            std::move(source_), std::forward<Action>(action)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Transform>
BasicStream<ReturnType<Transform, T>, P> BasicStream<T, P>::map(
            Transform&& transform) {

    using Result = ReturnType<Transform, T>;
    auto new_source = StreamProviderPtr<Result, P>(
        new MappedStreamProvider<Result, P, Transform>(
            std::move(source_), std::forward<Transform>(transform)));
    return BasicStream<Result, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
BasicStream<T, P> BasicStream<T, P>::limit(std::size_t length) {

    auto new_source = StreamProviderPtr<T, P>(
        new LimitedStreamProvider<T, P>(std::move(source_), length));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
BasicStream<T, P> BasicStream<T, P>::skip(std::size_t amount) {

    auto new_source = StreamProviderPtr<T, P>(
        new SkippedStreamProvider<T, P>(std::move(source_), amount));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Compare>
BasicStream<T, P> BasicStream<T, P>::sort(Compare&& comparator) {

    auto new_source = StreamProviderPtr<T, P>(
        new SortedStreamProvider<T, P, Compare>(
            std::move(source_), std::forward<Compare>(comparator)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Compare>
BasicStream<T, P> BasicStream<T, P>::distinct(Compare&& comparator) {

    auto new_source = StreamProviderPtr<T, P>(
        new DistinctStreamProvider<T, P, Compare>(
            std::move(source_), std::forward<Compare>(comparator)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Iterator>
BasicStream<T, P> BasicStream<T, P>::extend(Iterator begin, Iterator end) {
    return extend(BasicStream<T, P>(begin, end));
}

template<typename T, template<typename> class P>
BasicStream<T, P> BasicStream<T, P>::extend(BasicStream<T, P>&& other) {

    auto new_source = StreamProviderPtr<T, P>(
        new ConcatenatedStreamProvider<T, P>(
            std::move(source_), std::move(other.source_)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Other>
BasicStream<ZipResult<T, Other>, P> BasicStream<T, P>::zip(
            BasicStream<Other, P>&& other) {

    using Result = ZipResult<T, Other>;
    auto new_source = StreamProviderPtr<Result, P>(
        new ZippedStreamProvider<T, Other, P>(
            std::move(source_), std::move(other.source_)));
    return BasicStream<Result, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename OutputIterator>
void BasicStream<T, P>::copy_to(OutputIterator out) {
    while(source_->advance()) {
        *out = *source_->get();
        out++;
    }
}

template<typename T, template<typename> class P>
std::vector<T> BasicStream<T,P>::as_vector() {
    std::vector<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T, template<typename> class P>
std::list<T> BasicStream<T,P>::as_list() {
    std::list<T> result;
    copy_to(back_inserter(result));
    return result;
}

template<typename T, template<typename> class P>
template<typename Function>
void BasicStream<T, P>::for_each(Function&& function) {
    while(source_->advance()) {
        function(*source_->get());
    }
}

template<typename T, template<typename> class P>
size_t BasicStream<T, P>::count() {
    size_t count = 0;
    while(source_->advance()) {
        source_->get();
        count++;
    }
    return count;
}


#endif
