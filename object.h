#pragma once

#include <iostream>
#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::string ToString() {
        return "nothing";
    }

    virtual std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) {
    }
};

class Number : public Object {
public:
    Number(int64_t num) : num_(num) {
    }

    int GetValue() const {
        return num_;
    }

    std::string ToString() override {
        return std::to_string(num_);
    }

private:
    int64_t num_;
};

class Symbol : public Object {
public:
    Symbol(std::string str) : str_(str) {
    }

    const std::string& GetName() const {
        return str_;
    }

    std::string ToString() override {
        return str_;
    }

private:
    std::string str_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object>& first, std::shared_ptr<Object>& second) {
        first_ = first;
        second_ = second;
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }
    std::string ToString() override {
        if (first_ == nullptr && second_ == nullptr) {
            return "(())";
        }
        if (second_ == nullptr) {
            return "(" + first_->ToString() + ")";
        }
        if (dynamic_pointer_cast<Number>(second_) || dynamic_pointer_cast<Symbol>(second_)) {
            return "(" + first_->ToString() + " . " + second_->ToString() + ")";
        }

        std::string ans = "(" + first_->ToString();
        std::shared_ptr<Object> rest = second_;
        while (rest != nullptr) {
            if (auto cell = dynamic_pointer_cast<Cell>(rest)) {
                ans += " " + cell->GetFirst()->ToString();
                rest = cell->GetSecond();
            } else {
                ans += " . " + rest->ToString();
                break;
            }
        }
        ans += ")";
        return ans;
    }

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return As<T>(obj) != nullptr;
}
