#ifndef PEEK_STREAM_PROVIDER_H
#define PEEK_STREAM_PROVIDER_H

template<typename T, typename Action>
class PeekStreamProvider : public StreamProvider<T> {

public:
    PeekStreamProvider(StreamProviderPtr<T> source, Action&& action)
        : source_(std::move(source)), action_(action) {}

    std::shared_ptr<T> get() override {
        return current_;
    }

    bool advance() override {
        if(source_->advance()) {
            current_ = source_->get();
            action_(*current_);
            return true;
        }
        current_.reset();
        return false;
    }

private:
    StreamProviderPtr<T> source_;
    Action action_;
    std::shared_ptr<T> current_;

};

#endif
