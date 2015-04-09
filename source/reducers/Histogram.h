#ifndef SCHEINERMAN_STREAM_REDUCER_HISTOGRAM_H
#define SCHEINERMAN_STREAM_REDUCER_HISTOGRAM_H

#include "Reducer.h"

#include <map>

namespace reducers {

template<typename In, typename Less = std::less<In>>
class Histogram : public Reducer<In, std::map<In, size_t, Less>> {

public:
    using Map = std::map<In, size_t, Less>;

    Histogram(Less&& less = Less()) : less_(less) {}

    Map initial(In&& in) const override {
        Map histogram(less_);
        histogram[in]++;
        return histogram;
    }

    Map accumulate(Map&& result, In&& value) const override {
        result[value]++;
        return result;
    }

private:
    Less less_;

};

}


#endif
