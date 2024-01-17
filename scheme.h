#pragma once

#include "parser.h"
#include <deque>

inline std::unordered_map<std::string, std::shared_ptr<Object>> global_scope;

struct Context {
    std::unordered_map<std::string, std::shared_ptr<Object>> scope;
    Context* next = nullptr;
    Context* prev = nullptr;
};

inline Context context;

inline std::unordered_map<std::string, std::shared_ptr<Object>> scope_for_lambda;

inline bool check_scope = false;

inline std::unordered_map<std::string, std::shared_ptr<Object>> lambda_var_scope;

inline std::deque<int64_t> lambda_var;

class AddFunction : public Object {
public:
    AddFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class SubFunction : public Object {
public:
    SubFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MultiplyFunction : public Object {
public:
    MultiplyFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class DivisionFunction : public Object {
public:
    DivisionFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class LessFunction : public Object {
public:
    LessFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MoreFunction : public Object {
public:
    MoreFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class EqualFunction : public Object {
public:
    EqualFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class LessOrEqualFunction : public Object {
public:
    LessOrEqualFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MoreOrEqualFunction : public Object {
public:
    MoreOrEqualFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MaxFunction : public Object {
public:
    MaxFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MinFunction : public Object {
public:
    MinFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class AbsFunction : public Object {
public:
    AbsFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class NumberFunction : public Object {
public:
    NumberFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class BooleanFunction : public Object {
public:
    BooleanFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class NotFunction : public Object {
public:
    NotFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class AndFunction : public Object {
public:
    AndFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class OrFunction : public Object {
public:
    OrFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class QuoteFunction : public Object {
public:
    QuoteFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class PairFunction : public Object {
public:
    PairFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class NullFunction : public Object {
public:
    NullFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class ListFunction : public Object {
public:
    ListFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class ConsFunction : public Object {
public:
    ConsFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class CarFunction : public Object {
public:
    CarFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class CdrFunction : public Object {
public:
    CdrFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class MakeListFunction : public Object {
public:
    MakeListFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class ListRefFunction : public Object {
public:
    ListRefFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class ListTailFunction : public Object {
public:
    ListTailFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class SymbolFunction : public Object {
public:
    SymbolFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class DefineFunction : public Object {
public:
    DefineFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class SetFunction : public Object {
public:
    SetFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class IfFunction : public Object {
public:
    IfFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class SetCarFunction : public Object {
public:
    SetCarFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class SetCdrFunction : public Object {
public:
    SetCdrFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class LambdaFunction : public Object {
public:
    LambdaFunction() = default;
    std::shared_ptr<Object> Applay(std::shared_ptr<Object> obj) override;
};

class Interpreter {
public:
    Interpreter() {
        global_scope.clear();
        scope_for_lambda.clear();
        lambda_var.clear();
        context.scope.clear();
        context.next = nullptr;
        context.prev = nullptr;
        check_scope = false;
    }
    std::string Run(const std::string&);
};
