#ifndef SCHEINERMAN_STREAM_REDUCER_SUMMARY_STATS_H
#define SCHEINERMAN_STREAM_REDUCER_SUMMARY_STATS_H

#include "Reducer.h"

namespace reducers {

template<typename In, typename Out> class SummaryStats;

template<typename In, typename Out = double>
class Stats {

public:
    Stats(const Stats<In, Out>&) = default;
    Stats(Stats<In, Out>&&) = default;
    Stats<In,Out>& operator= (const Stats<In, Out>&) = default;
    Stats<In,Out>& operator= (Stats<In, Out>&&) = default;

    size_t number() const { return number_; }
    Out mean() const { return mean_;   }
    Out stddev() const {
        return (number_ > 1) ? variance_ / (number_ - 1) : 0;
    }
    In min() const { return min_; }
    In max() const { return max_; }

    friend class SummaryStats<In, Out>;

private:
    Stats(In value)
        : number_(1), mean_(value), variance_(0), min_(value), max_(value) {}

    Stats<In, Out> accept(In&& value) {
        number_++;
        double prev_mean = mean_;
        mean_ += (value - mean_) / number_;
        variance_ += (value - prev_mean) * (value - mean_);
        if(value < min_)
            min_ = value;
        if(value > max_)
            max_ = value;
        return *this;
    }

    size_t number_;
    Out mean_;
    Out variance_;
    Out stddev_;
    In max_;
    In min_;
};

template<typename In, typename Out>
std::ostream& operator<< (std::ostream& os, const Stats<In, Out>& stats) {
    return os << "N=" << stats.number()
              << ", u=" << stats.mean()
              << ", s=" << stats.stddev()
              << ", min=" << stats.min()
              << ", max=" << stats.max();
}

template<typename In, typename Result=double>
class SummaryStats : public Reducer<In, Stats<In, Result>> {

public:
    using Out = Stats<In, Result>;

    Out initial(In&& in) const override {
        return Out(std::forward<In>(in));
    }

    Out accumulate(Out&& out, In&& in) const override {
        return out.accept(std::forward<In>(in));
    }
};

}

#endif
