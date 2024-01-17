#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

class ControlBlockBase {
public:
    virtual ~ControlBlockBase() {
    }
    virtual void IncCounterStrong() {
    }
    virtual void DecCounterStrong() {
    }
    virtual void IncCounterWeak() {
    }
    virtual void DecCounterWeak() {
    }
    virtual size_t CurrCounter() = 0;
};

template <typename T>
class ControlBlockObject : public ControlBlockBase {
public:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
    size_t counter_strong_;
    size_t counter_weak_ = 0;

    void IncCounterStrong() override {
        ++counter_strong_;
    }
    void DecCounterStrong() override {
        --counter_strong_;
        if (counter_strong_ == 0 && counter_weak_ == 0) {
            reinterpret_cast<T*>(&storage_)->~T();
            delete this;
        } else if (counter_strong_ == 0) {
            reinterpret_cast<T*>(&storage_)->~T();
        }
    }
    void IncCounterWeak() override {
        ++counter_weak_;
    }
    void DecCounterWeak() override {
        --counter_weak_;
        if (counter_weak_ == 0 && counter_strong_ == 0) {
            delete this;
        }
    }
    size_t CurrCounter() override {
        return counter_strong_;
    }
    template <typename... Args>
    ControlBlockObject(Args&&... args) : counter_strong_(1) {
        new (&storage_) T(std::forward<Args>(args)...);
    }
    T* GetPtr() {
        return reinterpret_cast<T*>(&storage_);
    }
    ~ControlBlockObject() override = default;
};

template <typename T>
class ControlBlockPtr : public ControlBlockBase {
public:
    size_t counter_strong_;
    size_t counter_weak_ = 0;
    T* val_;
    ControlBlockPtr(T* val) : counter_strong_(1), val_(val) {
    }
    void IncCounterStrong() override {
        ++counter_strong_;
    }
    void DecCounterStrong() override {
        --counter_strong_;
        if (counter_strong_ == 0 && counter_weak_ == 0) {
            delete val_;
            delete this;
        } else if (counter_strong_ == 0) {
            delete val_;
        }
    }
    void IncCounterWeak() override {
        ++counter_weak_;
    }
    void DecCounterWeak() override {
        --counter_weak_;
        if (counter_weak_ == 0 && counter_strong_ == 0) {
            delete this;
        }
    }
    size_t CurrCounter() override {
        return counter_strong_;
    }
    ~ControlBlockPtr() override = default;
};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
