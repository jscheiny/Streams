#ifndef SCHEINERMAN_STREAM_REDUCER_H
#define SCHEINERMAN_STREAM_REDUCER_H

#include "../Stream.h"

#include <functional>

template<typename In, typename Out>
struct Reducer {
    virtual Out initial(In&& in) const = 0;
    virtual Out accumulate(Out&& out, In&& in) const = 0;
    auto reducer() const {
        using namespace std::placeholders;
        return stream::op::reduce(
            std::bind(&Reducer<In, Out>::initial, this, _1),
            std::bind(&Reducer<In, Out>::accumulate, this, _1, _2)
        );
    }
};

#endif
