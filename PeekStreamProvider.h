#ifndef PEEK_STREAM_PROVIDER_H
#define PEEK_STREAM_PROVIDER_H

template<typename T, template<typename> class Pointer, typename Action>
class PeekStreamProvider : public StreamProvider<T, Pointer> {

public:
    PeekStreamProvider(StreamProviderPtr<T, Pointer> source, Action&& action)
        : source_(std::move(source)), action_(action) {}

    Pointer<T> Next() override {
        return std::move(next_);
    }

    bool HasNext() override {
        if(source_->HasNext()) {
            next_ = source_->Next();
            action_(*next_);
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Action action_;
    Pointer<T> next_;

};

#endif
