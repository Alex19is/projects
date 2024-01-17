#pragma once

#include "sw_fwd.h"  // Forward declaration

template <typename T>
class WeakPtr {
public:
    ControlBlockBase* obj_;
    T* observed_;
    // Constructors

    WeakPtr() noexcept : obj_(nullptr), observed_(nullptr) {
    }

    WeakPtr(const WeakPtr& other) noexcept : obj_(other.obj_), observed_(other.observed_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounterWeak();
        }
    }

    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) noexcept : obj_(other.obj_), observed_(other.observed_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounterWeak();
        }
    }

    template <typename Y>
    WeakPtr(WeakPtr<Y>&& other)
        : obj_(std::move(other.obj_)), observed_(std::move(other.observed_)) {
        other.obj_ = nullptr;
        other.observed_ = nullptr;
    }

    WeakPtr(WeakPtr&& other) noexcept
        : obj_(std::move(other.obj_)), observed_(std::move(other.observed_)) {
        other.obj_ = nullptr;
        other.observed_ = nullptr;
    }

    template <typename Y>
    WeakPtr(const SharedPtr<Y>& other) noexcept : obj_(other.obj_), observed_(other.observed_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounterWeak();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        WeakPtr<T>(other).Swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(const WeakPtr<Y>& other) noexcept {
        WeakPtr<T>(other).Swap(*this);
        return *this;
    }

    template <typename Y>
    WeakPtr& operator=(WeakPtr<Y>&& other) noexcept {
        WeakPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        WeakPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    ~WeakPtr() {
        observed_ = nullptr;
        if (obj_ != nullptr) {
            obj_->DecCounterWeak();
            obj_ = nullptr;
        }
    }

    void Reset() noexcept {
        if (obj_ != nullptr) {
            obj_->DecCounterWeak();
        }
        obj_ = nullptr;
        observed_ = nullptr;
    }
    void Swap(WeakPtr& other) {
        std::swap(obj_, other.obj_);
        std::swap(observed_, other.observed_);
    }

    size_t UseCount() const noexcept {
        if (obj_ == nullptr) {
            return 0;
        }
        return obj_->CurrCounter();
    }
    bool Expired() const noexcept {
        return (obj_ == nullptr) || (obj_->CurrCounter() == 0);
    }
    SharedPtr<T> Lock() const noexcept {
        if (Expired()) {
            return SharedPtr<T>();
        } else {
            return SharedPtr<T>(*this);
        }
    }
};
