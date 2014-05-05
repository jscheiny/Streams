#ifndef MERGED_STREAM_PROVIDER_H
#define MERGED_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

template<typename T, typename Compare>
class MergedStreamProvider : public StreamProvider<T> {

public:
    MergedStreamProvider(StreamProviderPtr<T> source1,
                         StreamProviderPtr<T> source2,
                         Compare&& comparator,
                         bool allow_duplicates)
        : source1_(std::move(source1)),
          source2_(std::move(source2)),
          comparator_(comparator),
          allow_duplicates_(allow_duplicates) {}

    std::shared_ptr<T> get() override {
        return result_;
    }

    bool advance() override {
        if(depletion_ == BothDepleted) {
            return false;
        }

        perform_advance();
        return perform_update();
    }

private:
    enum ToAdvance {
        FirstSource,
        SecondSource,
        BothSources
    };

    enum DepleteState {
        NeitherDepleted,
        FirstDepleted,
        SecondDepleted,
        BothDepleted
    };

    ToAdvance advance_ = BothSources;
    DepleteState depletion_ = NeitherDepleted;

    Compare comparator_;
    StreamProviderPtr<T> source1_;
    StreamProviderPtr<T> source2_;
    std::shared_ptr<T> current1_;
    std::shared_ptr<T> current2_;
    std::shared_ptr<T> result_;
    bool allow_duplicates_;

    bool advance_stream(StreamProviderPtr<T>& source,
                        std::shared_ptr<T>& current,
                        DepleteState deplete_state,
                        ToAdvance advance_result) {
        if(source->advance()) {
            current = source->get();
            return true;
        } else {
            current.reset();
            if(depletion_ == NeitherDepleted) {
                depletion_ = deplete_state;
                advance_ = advance_result;
            } else {
                depletion_ = BothDepleted;
            }
            return false;
        }
    }

    void perform_advance() {
        switch(advance_) {
        case FirstSource:
            advance_stream(source1_, current1_, FirstDepleted, SecondSource);
            break;
        case SecondSource:
            advance_stream(source2_, current2_, SecondDepleted, FirstSource);
            break;
        case BothSources:
            advance_stream(source1_, current1_, FirstDepleted, SecondSource);
            advance_stream(source2_, current2_, SecondDepleted, FirstSource);
            break;
        }
    }

    bool perform_update() {
        switch(depletion_) {
        case NeitherDepleted:
            if(allow_duplicates_) {
                duplicates_update();
            } else {
                no_duplicates_update();
            }
            return true;
        case FirstDepleted:
            result_ = current2_;
            return true;
        case SecondDepleted:
            result_ = current1_;
            return true;
        case BothDepleted:
            return false;
        }
    }

    void duplicates_update() {
        if(comparator_(*current1_, *current2_)) { // curr1 < curr2
            advance_ = FirstSource;
            result_ = current1_;
        } else {
            advance_ = SecondSource;
            result_ = current2_;
        }
    }

    void no_duplicates_update() {
        if(comparator_(*current1_, *current2_)) { // curr1 < curr2
            advance_ = FirstSource;
            result_ = current1_;
        } else if(comparator_(*current2_, *current1_ )) { // curr2 < curr1
            advance_ = SecondSource;
            result_ = current2_;
        } else {
            advance_ = BothSources;
            result_ = current1_;
        }
    }

};

#endif
