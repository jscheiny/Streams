#ifndef MERGED_STREAM_PROVIDER_H
#define MERGED_STREAM_PROVIDER_H

#include "StreamProvider.h"
#include "Utility.h"

enum DepleteState {
    NeitherDepleted,
    FirstDepleted,
    SecondDepleted,
    BothDepleted
};


std::ostream& operator<<(std::ostream& os, DepleteState deplete) {
    switch(deplete) {
    case NeitherDepleted:
        return os << "Neither depleted";
    case FirstDepleted:
        return os << "First depleted";
    case SecondDepleted:
        return os << "Second depleted";
    case BothDepleted:
        return os << "Both depleted";
    }

}


template<typename T, typename Compare>
class MergedStreamProvider : public StreamProvider<T> {

public:
    MergedStreamProvider(StreamProviderPtr<T> source1,
                         StreamProviderPtr<T> source2,
                         Compare&& comparator)
        : source1_(std::move(source1)),
          source2_(std::move(source2)),
          comparator_(comparator) {}

    std::shared_ptr<T> get() override {
        if(!result_) {
            std::cout << "UH OH" << std::endl;
        }
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


    ToAdvance advance_ = BothSources;
    DepleteState depletion_ = NeitherDepleted;

    Compare comparator_;

    StreamProviderPtr<T> source1_;
    StreamProviderPtr<T> source2_;

    std::shared_ptr<T> current1_;
    std::shared_ptr<T> current2_;

    std::shared_ptr<T> result_;

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
            if(comparator_(*current1_, *current2_)) { // curr1 < curr2
                advance_ = FirstSource;
                result_ = current1_;
            } else {
                advance_ = SecondSource;
                result_ = current2_;
            }
            return true;
        case FirstDepleted:
            current1_.reset();
            result_ = current2_;
            return true;
        case SecondDepleted:
            current2_.reset();
            result_ = current1_;
            return true;
        case BothDepleted:
            current1_.reset();
            current2_.reset();
            return false;
        }
    }

};

#endif
