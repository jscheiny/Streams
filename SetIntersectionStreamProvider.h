#ifndef SET_INTERSECTION_STREAM_PROVIDER_H
#define SET_INTERSECTION_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Compare>
class SetIntersectionStreamProvider : public StreamProvider<T> {

public:
    SetIntersectionStreamProvider(StreamProviderPtr<T> source1,
                                  StreamProviderPtr<T> source2,
                                  Compare&& comparator)
        : source1_(std::move(source1)),
          source2_(std::move(source2)),
          comparator_(comparator) {}

    std::shared_ptr<T> get() override {
        return result_;
    }

    bool advance() override {
        if(depletion_ == EitherDepleted) {
            return false;
        }

        advance_stream(source1_, current1_);
        advance_stream(source2_, current2_);

        while(depletion_ == NeitherDepleted) {
            if(comparator_(*current1_, *current2_)) { // curr1 < curr2
                advance_stream(source1_, current1_);
            } else if(comparator_(*current2_, *current1_)) { // curr2 < curr1
                advance_stream(source2_, current2_);
            } else { // curr1 == curr2
                result_ = current1_;
                return true;
            }
        }
        return false;
    }

private:
    enum DepleteState {
        NeitherDepleted,
        EitherDepleted
    };

    DepleteState depletion_ = NeitherDepleted;

    Compare comparator_;
    StreamProviderPtr<T> source1_;
    StreamProviderPtr<T> source2_;
    std::shared_ptr<T> current1_;
    std::shared_ptr<T> current2_;
    std::shared_ptr<T> result_;

    bool advance_stream(StreamProviderPtr<T>& source,
                        std::shared_ptr<T>& current) {
        if(source->advance()) {
            current = source->get();
            return true;
        } else {
            current.reset();
            depletion_ = EitherDepleted;
            return false;
        }
    }

};

#endif
