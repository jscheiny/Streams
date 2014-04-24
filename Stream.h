#ifndef STREAM_H
#define STREAM_H

#include "Providers.h"

#include <functional>

template<typename T, template<typename> class Pointer>
class BasicStream {

public:
    template<typename Iterator>
    BasicStream(Iterator begin, Iterator end)
        : source_(std::make_unique<IteratorStreamProvider<T, Pointer, Iterator>>(
            begin, end)) {}

    template<typename Predicate>
    BasicStream<T, Pointer> filter(Predicate&& predicate) {
        auto new_source = std::unique_ptr<StreamProvider<T, Pointer>>(
            new FilteredStreamProvider<T, Pointer, Predicate>(
                std::move(source_), std::forward<Predicate>(predicate)));
        return BasicStream<T, Pointer>(std::move(new_source));
    }

    template<typename Transform, typename R = decltype(std::declval<Transform>()(std::declval<T>()))>
    BasicStream<R, Pointer> map(Transform&& transform) {
        auto new_source = std::unique_ptr<StreamProvider<R, Pointer>>(
            new MappedStreamProvider<R, Pointer, Transform>(
                std::move(source_), std::forward<Transform>(transform)));
        return BasicStream<T, Pointer>(std::move(new_source));
    }

    BasicStream<T, Pointer> limit(std::size_t length) {
        auto new_source = std::unique_ptr<StreamProvider<T, Pointer>>(
            new LimitedStreamProvider<T, Pointer>(std::move(source_), length));
        return BasicStream<T, Pointer>(std::move(new_source));
    }

    template<typename OutputIterator>
    void copy_to(OutputIterator out) {
        while(source_->HasNext()) {
            *out = *source_->Next();
            out++;
        }
    }

private:
    BasicStream(StreamProviderPtr<T, Pointer> source)
        : source_(std::move(source)) {}

    StreamProviderPtr<T, Pointer> source_;

};

template<typename T> using UniquePtr = std::unique_ptr<T>;
template<typename T> using SharedPtr = std::shared_ptr<T>;

template<typename T> using UniqueStream = BasicStream<T, UniquePtr>;
template<typename T> using SharedStream = BasicStream<T, SharedPtr>;

template<typename T> using Stream = UniqueStream<T>;


#endif
