#ifndef REDUCER_H
#define REDUCER_H

template<typename In, typename Out>
struct Reducer {
    virtual Out initial(In&& in) const = 0;
    virtual Out accumulate(Out&& out, In&& in) const = 0;
};

#endif