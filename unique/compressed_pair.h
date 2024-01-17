#include <iostream>
#include <type_traits>
#include <utility>
#include <cstddef>  // std::nullptr_t
#include <vector>
#include <tuple>

template <typename T, std::size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct Element {
    T now;
    const T& Get() const {
        return now;
    }
    T& Get() {
        return now;
    }
};

template <typename T, std::size_t I>
struct Element<T, I, true> : public T {
    const T& Get() const {
        return *this;
    }
    T& Get() {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private Element<F, 0>, private Element<S, 1> {
public:
    CompressedPair() : Element<F, 0>{}, Element<S, 1>{} {
    }

    CompressedPair(F&& first, S&& second)
        : Element<F, 0>{std::forward<F>(first)}, Element<S, 1>{std::forward<S>(second)} {
    }

    CompressedPair(const F& first, const S& second) : Element<F, 0>{first}, Element<S, 1>{second} {
    }

    CompressedPair(const F& first, S&& second)
        : Element<F, 0>{first}, Element<S, 1>{std::forward<S>(second)} {
    }

    F& GetFirst() {
        return Element<F, 0>::Get();
    }

    const F& GetFirst() const {
        return Element<F, 0>::Get();
    }

    S& GetSecond() {
        return Element<S, 1>::Get();
    }

    const S& GetSecond() const {
        return Element<S, 1>::Get();
    }
};
