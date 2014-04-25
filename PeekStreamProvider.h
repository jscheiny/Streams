#ifndef PEEK_STREAM_PROVIDER_H
#define PEEK_STREAM_PROVIDER_H

template<typename T, template<typename> class Pointer, typename Action>
class PeekStreamProvider : public StreamProvider<T, Pointer> {

public:
    PeekStreamProvider(StreamProviderPtr<T, Pointer> source, Action&& action)
        : source_(std::move(source)), action_(action) {}

    Pointer<T> get() override {
        return std::move(current_);
    }

    bool advance() override {
        if(source_->advance()) {
            current_ = source_->get();
            action_(*current_);
            return true;
        }
        return false;
    }

private:
    StreamProviderPtr<T, Pointer> source_;
    Action action_;
    Pointer<T> current_;

};

#endif
