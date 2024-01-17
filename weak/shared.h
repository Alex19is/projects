#pragma once

#include <cstddef>
#include "sw_fwd.h"
#include <memory>
#include <algorithm>

template <typename T>
class SharedPtr {
public:
    ControlBlockBase* obj_;
    T* observed_;

    SharedPtr() noexcept : obj_(nullptr), observed_(nullptr) {
    }
    SharedPtr(std::nullptr_t) noexcept : obj_(nullptr), observed_(nullptr) {
    }
    template <typename Y>
    explicit SharedPtr(Y* ptr) : obj_(new ControlBlockPtr<Y>(ptr)), observed_(ptr) {
    }
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) noexcept : obj_(other.obj_), observed_(other.observed_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounterStrong();
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : obj_(other.obj_), observed_(other.observed_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounterStrong();
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) noexcept
        : obj_(std::move(other.obj_)), observed_(std::move(other.observed_)) {
        other.obj_ = nullptr;
        other.observed_ = nullptr;
    }

    SharedPtr(SharedPtr&& other) noexcept
        : obj_(std::move(other.obj_)), observed_(std::move(other.observed_)) {
        other.obj_ = nullptr;
        other.observed_ = nullptr;
    }

    // Aliasing constructor
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : obj_(other.obj_), observed_(ptr) {
        if (obj_) {
            obj_->IncCounterStrong();
        }
    }

    SharedPtr(ControlBlockObject<T>* obj, T* ptr) {
        obj_ = obj;
        observed_ = ptr;
    }

    explicit SharedPtr(const WeakPtr<T>& other) : obj_(other.obj_), observed_(other.observed_) {
        if (obj_ == nullptr || obj_->CurrCounter() == 0) {
            throw BadWeakPtr();
        }
        obj_->IncCounterStrong();
    }

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr<T>(other).Swap(*this);
        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) noexcept {
        SharedPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr<T>(std::move(other)).Swap(*this);
        return *this;
    }

    ~SharedPtr() {
        if (obj_ != nullptr) {
            obj_->DecCounterStrong();
        }
    }

    void Reset() noexcept {
        SharedPtr().Swap(*this);
    }
    template <typename Y>
    void Reset(Y* ptr) {
        SharedPtr<T>(ptr).Swap(*this);
    }
    void Swap(SharedPtr& other) {
        std::swap(obj_, other.obj_);
        std::swap(observed_, other.observed_);
    }

    T* Get() const {
        return observed_;
    }
    T& operator*() const {
        return *observed_;
    }
    T* operator->() const {
        return observed_;
    }
    size_t UseCount() const {
        if (obj_ == nullptr) {
            return 0;
        }
        return obj_->CurrCounter();
    }
    explicit operator bool() const {
        return obj_ != nullptr;
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    auto curr = new ControlBlockObject<T>(std::forward<Args>(args)...);
    SharedPtr<T> res(curr, curr->GetPtr());
    res.obj_ = curr;
    res.observed_ = curr->GetPtr();
    return res;
}

template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
