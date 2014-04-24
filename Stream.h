#ifndef STREAM_H
#define STREAM_H

#include "Providers.h"
#include "Utility.h"

#include <functional>
#include <type_traits>

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
    static BasicStream<T, Pointer> Generate(Generator&& generator);

    template<typename Iterator>
    BasicStream(Iterator begin, Iterator end)
        : source_(std::make_unique<IteratorStreamProvider<T, Pointer, Iterator>>(
            begin, end)) {}

    template<typename Predicate>
    BasicStream<T, Pointer> Filter(Predicate&& predicate);

    template<typename Action>
    BasicStream<T, Pointer> Peek(Action&& action);

    template<typename Transform, typename R = ReturnType<Transform, T>>
    BasicStream<R, Pointer> Map(Transform&& transform);


    BasicStream<T, Pointer> Limit(std::size_t length);

    template<typename OutputIterator>
    void CopyTo(OutputIterator out);

private:
    BasicStream(StreamProviderPtr<T, Pointer> source)
        : source_(std::move(source)) {}

    StreamProviderPtr<T, Pointer> source_;

};

template<typename T, template<typename> class P>
template<typename Generator>
BasicStream<T, P> BasicStream<T, P>::Generate(Generator&& generator) {

    auto source = StreamProviderPtr<T, P>(
        new GeneratedStreamProvider<T, P, Generator>(
            std::forward<Generator>(generator)));
    return BasicStream<T, P>(std::move(source));

}

template<typename T, template<typename> class P>
template<typename Predicate>
BasicStream<T, P> BasicStream<T, P>::Filter(Predicate&& predicate) {

    auto new_source = StreamProviderPtr<T, P>(
        new FilteredStreamProvider<T, P, Predicate>(
            std::move(source_), std::forward<Predicate>(predicate)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Action>
BasicStream<T, P> BasicStream<T, P>::Peek(Action&& action) {

    auto new_source = StreamProviderPtr<T, P>(
        new PeekStreamProvider<T, P, Action>(
            std::move(source_), std::forward<Action>(action)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename Transform, typename R>
BasicStream<R, P> BasicStream<T, P>::Map(Transform&& transform) {

    auto new_source = StreamProviderPtr<T, P>(
        new MappedStreamProvider<R, P, Transform>(
            std::move(source_), std::forward<Transform>(transform)));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
BasicStream<T, P> BasicStream<T, P>::Limit(std::size_t length) {

    auto new_source = StreamProviderPtr<T, P>(
        new LimitedStreamProvider<T, P>(std::move(source_), length));
    return BasicStream<T, P>(std::move(new_source));

}

template<typename T, template<typename> class P>
template<typename OutputIterator>
void BasicStream<T, P>::CopyTo(OutputIterator out) {
    while(source_->HasNext()) {
        *out = *source_->Next();
        out++;
    }
}

#endif
