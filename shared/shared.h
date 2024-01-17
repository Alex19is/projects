#include <cstddef>
#include "sw_fwd.h"
#include <memory>
#include <algorithm>

class ControlBlockBase {
public:
    virtual ~ControlBlockBase() {
    }
    virtual void IncCounter() {
    }
    virtual void DecCounter() {
    }
    virtual size_t CurrCounter() = 0;
};

template <typename T>
class ControlBlockObject : public ControlBlockBase {
public:
    std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
    size_t counter_;

    void IncCounter() override {
        ++counter_;
    }
    void DecCounter() override {
        --counter_;
    }
    size_t CurrCounter() override {
        return counter_;
    }

    template <typename... Args>
    ControlBlockObject(Args&&... args) : counter_(1) {
        new (&storage_) T(std::forward<Args>(args)...);
    }

    T* GetPtr() {
        return reinterpret_cast<T*>(&storage_);
    }
    ~ControlBlockObject() override {
        reinterpret_cast<T*>(&storage_)->~T();
    }
};

template <typename T>
class ControlBlockPtr : public ControlBlockBase {
public:
    size_t counter_;
    T* val_;
    ControlBlockPtr(T* val) : counter_(1), val_(val) {
    }
    void IncCounter() override {
        ++counter_;
    }
    void DecCounter() override {
        --counter_;
    }
    size_t CurrCounter() override {
        return counter_;
    }
    ~ControlBlockPtr() override {
        delete val_;
    }
};

template <typename T>
class SharedPtr {
public:
    ControlBlockBase* obj_;
    T* observer_;

    SharedPtr() noexcept : obj_(nullptr), observer_(nullptr) {
    }
    SharedPtr(std::nullptr_t) noexcept : obj_(nullptr), observer_(nullptr) {
    }
    template <typename Y>
    explicit SharedPtr(Y* ptr) : obj_(new ControlBlockPtr<Y>(ptr)), observer_(ptr) {
    }
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) noexcept : obj_(other.obj_), observer_(other.observer_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounter();
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : obj_(other.obj_), observer_(other.observer_) {
        if (other.obj_ != nullptr) {
            obj_->IncCounter();
        }
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) noexcept
        : obj_(std::move(other.obj_)), observer_(std::move(other.observer_)) {
        other.obj_ = nullptr;
        other.observer_ = nullptr;
    }

    SharedPtr(SharedPtr&& other) noexcept
        : obj_(std::move(other.obj_)), observer_(std::move(other.observer_)) {
        other.obj_ = nullptr;
        other.observer_ = nullptr;
    }

    // Aliasing constructor
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : obj_(other.obj_), observer_(ptr) {
        if (obj_) {
            obj_->IncCounter();
        }
    }

    SharedPtr(ControlBlockObject<T>* obj, T* ptr) {
        obj_ = obj;
        observer_ = ptr;
    }

    explicit SharedPtr(const WeakPtr<T>& other);

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
            obj_->DecCounter();
            if (obj_->CurrCounter() == 0) {
                delete obj_;
            }
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
        std::swap(observer_, other.observer_);
    }

    T* Get() const {
        return observer_;
    }
    T& operator*() const {
        return *observer_;
    }
    T* operator->() const {
        return observer_;
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
    res.observer_ = curr->GetPtr();
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
