#pragma once

#include "compressed_pair.h"

#include <utility>
#include <iostream>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <vector>
#include <tuple>

struct Slug {
    template <typename T>
    void operator()(T*) {
    }
};

template <typename T, class Deleter = Slug>
class UniquePtr {
public:
    template <typename X, typename Y>
    friend class UniquePtr;
    explicit UniquePtr(T* ptr = nullptr) noexcept {
        obj_.GetFirst() = ptr;
        obj_.GetSecond() = Deleter();
    }
    template <typename X, typename Y>
    UniquePtr(X* ptr, Y deleter) noexcept {
        obj_.GetFirst() = ptr;
        obj_.GetSecond() = std::move(deleter);
    }

    template <typename X, typename Y>
    UniquePtr(UniquePtr<X, Y>& other) = delete;

    UniquePtr(UniquePtr& other) = delete;

    template <typename X, typename Y>
    UniquePtr(UniquePtr<X, Y>&& other) noexcept {
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::move(other.obj_.GetSecond());
    }

    UniquePtr(UniquePtr&& other) noexcept {
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::move(other.obj_.GetSecond());
    }

    template <typename X, typename Y>
    UniquePtr& operator=(UniquePtr<X, Y>&& other) noexcept {
        if (obj_.GetFirst() == other.obj_.GetFirst()) {
            return *this;
        }
        if (obj_.GetFirst() != nullptr) {
            Reset(nullptr);
        }
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (obj_.GetFirst() == other.obj_.GetFirst()) {
            return *this;
        }
        if (obj_.GetFirst() != nullptr) {
            Reset(nullptr);
        }
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset(nullptr);
        return *this;
    }
    ~UniquePtr() noexcept {

        if (std::is_same<Deleter, Slug>::value) {
            delete obj_.GetFirst();
        } else {
            obj_.GetSecond()(obj_.GetFirst());
        }
    }

    T* Release() noexcept {
        T* ptr = obj_.GetFirst();
        obj_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) noexcept {
        auto copy = obj_.GetFirst();
        obj_.GetFirst() = ptr;
        if (copy) {
            if (std::is_same<Deleter, Slug>::value) {
                delete copy;
            } else {
                obj_.GetSecond()(copy);
            }
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(obj_.GetFirst(), other.obj_.GetFirst());
        std::swap(obj_.GetSecond(), other.obj_.GetSecond());
    }
    const T* Get() const noexcept {
        return obj_.GetFirst();
    };
    T* Get() noexcept {
        return obj_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return obj_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return obj_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return obj_.GetFirst() != nullptr;
    }
    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return *obj_.GetFirst();
    }
    T* operator->() const noexcept {
        return obj_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> obj_;
};

template <typename T, class Deleter>
class UniquePtr<T[], Deleter> {
public:
    template <typename X, typename Y>
    friend class UniquePtr;
    explicit UniquePtr(T* ptr = nullptr) noexcept {
        obj_.GetFirst() = ptr;
        obj_.GetSecond() = Deleter();
    }
    template <typename X, typename Y>
    UniquePtr(X* ptr, Y deleter) noexcept {
        obj_.GetFirst() = ptr;
        obj_.GetSecond() = std::move(deleter);
    }

    template <typename X, typename Y>
    UniquePtr(UniquePtr<X, Y>& other) = delete;

    UniquePtr(UniquePtr& other) = delete;

    template <typename X, typename Y>
    UniquePtr(UniquePtr<X, Y>&& other) noexcept {
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::move(other.obj_.GetSecond());
    }

    UniquePtr(UniquePtr&& other) noexcept {
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::move(other.obj_.GetSecond());
    }

    template <typename X, typename Y>
    UniquePtr& operator=(UniquePtr<X, Y>&& other) noexcept {
        if (obj_.GetFirst() == other.obj_.GetFirst()) {
            return *this;
        }
        if (obj_.GetFirst() != nullptr) {
            Reset(nullptr);
        }
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }

    T& operator[](size_t index) {
        return obj_.GetFirst()[index];
    }
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (obj_.GetFirst() == other.obj_.GetFirst()) {
            return *this;
        }
        if (obj_.GetFirst() != nullptr) {
            Reset(nullptr);
        }
        obj_.GetFirst() = other.Release();
        obj_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        Reset(nullptr);
        return *this;
    }
    ~UniquePtr() noexcept {

        if (std::is_same<Deleter, Slug>::value) {
            delete[] obj_.GetFirst();
        } else {
            obj_.GetSecond()(obj_.GetFirst());
        }
    }
    T* Release() noexcept {
        T* ptr = obj_.GetFirst();
        obj_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) noexcept {
        auto copy = obj_.GetFirst();
        obj_.GetFirst() = ptr;
        if (copy) {
            if (std::is_same<Deleter, Slug>::value) {
                delete[] copy;
            } else {
                obj_.GetSecond()(copy);
            }
        }
    }
    void Swap(UniquePtr& other) noexcept {
        std::swap(obj_.GetFirst(), other.obj_.GetFirst());
        std::swap(obj_.GetSecond(), other.obj_.GetSecond());
    }
    const T* Get() const noexcept {
        return obj_.GetFirst();
    };
    T* Get() noexcept {
        return obj_.GetFirst();
    }
    Deleter& GetDeleter() noexcept {
        return obj_.GetSecond();
    }
    const Deleter& GetDeleter() const noexcept {
        return obj_.GetSecond();
    }
    explicit operator bool() const noexcept {
        return obj_.GetFirst() == nullptr;
    }
    std::add_lvalue_reference_t<T> operator*() const noexcept {
        return *obj_.GetFirst();
    }
    T* operator->() const noexcept {
        return obj_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> obj_;
};
