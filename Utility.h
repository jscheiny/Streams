#ifndef UTILITY_H
#define UTILITY_H

template<typename T>
std::unique_ptr<T> move_unique(T&& t) {
    return std::make_unique<T>(std::move(t));
}

template<typename Transform, typename... Arg> using ReturnType =
    decltype(std::declval<Transform>()(std::declval<Arg>()...));

#endif
