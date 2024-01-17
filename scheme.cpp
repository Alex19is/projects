#include "scheme.h"
#include <cmath>
#include <limits>

std::unordered_map<std::string, std::shared_ptr<Object>> functionMap = {
    {"number?", std::make_shared<NumberFunction>()},
    {"abs", std::make_shared<AbsFunction>()},
    {"=", std::make_shared<EqualFunction>()},
    {">", std::make_shared<MoreFunction>()},
    {"<", std::make_shared<LessFunction>()},
    {">=", std::make_shared<MoreOrEqualFunction>()},
    {"<=", std::make_shared<LessOrEqualFunction>()},
    {"max", std::make_shared<MaxFunction>()},
    {"min", std::make_shared<MinFunction>()},
    {"+", std::make_shared<AddFunction>()},
    {"*", std::make_shared<MultiplyFunction>()},
    {"-", std::make_shared<SubFunction>()},
    {"/", std::make_shared<DivisionFunction>()},
    {"boolean?", std::make_shared<BooleanFunction>()},
    {"not", std::make_shared<NotFunction>()},
    {"and", std::make_shared<AndFunction>()},
    {"or", std::make_shared<OrFunction>()},
    {"quote", std::make_shared<QuoteFunction>()},
    {"null?", std::make_shared<NullFunction>()},
    {"pair?", std::make_shared<PairFunction>()},
    {"list?", std::make_shared<ListFunction>()},
    {"cons", std::make_shared<ConsFunction>()},
    {"car", std::make_shared<CarFunction>()},
    {"cdr", std::make_shared<CdrFunction>()},
    {"list", std::make_shared<MakeListFunction>()},
    {"list-ref", std::make_shared<ListRefFunction>()},
    {"list-tail", std::make_shared<ListTailFunction>()},
    {"symbol?", std::make_shared<SymbolFunction>()},
    {"define", std::make_shared<DefineFunction>()},
    {"set!", std::make_shared<SetFunction>()},
    {"if", std::make_shared<IfFunction>()},
    {"set-car!", std::make_shared<SetCarFunction>()},
    {"set-cdr!", std::make_shared<SetCdrFunction>()},
    {"lambda", std::make_shared<LambdaFunction>()}
};

// function quote
std::shared_ptr<Object> QuoteFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    return As<Cell>(curr)->GetFirst();
}

// Function number?
std::shared_ptr<Object> NumberFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (Is<Number>(As<Cell>(curr)->GetFirst())) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

// Function abs
std::shared_ptr<Object> AbsFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        throw RuntimeError("Too few args for abs");
    }
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (Is<Number>(As<Cell>(curr)->GetFirst())) {
        if (As<Cell>(curr)->GetSecond() != nullptr) {
            throw RuntimeError("Too many args for abs");
        }
        std::shared_ptr<Object> ans = As<Cell>(curr)->GetFirst();
        int64_t val = As<Number>(ans)->GetValue();
        if (val > 0) {
            return std::make_shared<Number>(val);
        }
        return std::make_shared<Number>(-val);
    }
    throw RuntimeError("Invalid Argument for abs");
}

// my general realisation for < > <= >= =
bool ComparatorOp(int64_t& a, int64_t& b, const std::string& op) {  // "template operator"
    if (op == "<") {
        return a < b;
    } else if (op == ">") {
        return a > b;
    } else if (op == "<=") {
        return a <= b;
    } else if (op == ">=") {
        return a >= b;
    } else {
        return a == b;
    }
}

std::shared_ptr<Object> ApplayForOp(std::shared_ptr<Object> obj, const std::string& op) {
    std::vector<int64_t> result;
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (curr == nullptr) {
        return std::make_shared<Symbol>("#t");
    }
    while (curr != nullptr) {
        std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
        if (Is<Cell>(first)) {
            std::shared_ptr<Object> func = As<Cell>(first)->GetFirst();
            if (func == nullptr) {
                throw RuntimeError("Bad syntax");
            }
            std::string func_name = func->ToString();
            if (functionMap.find(func_name) != functionMap.end()) {
                std::shared_ptr<Object> func1 = functionMap[func_name];
                std::shared_ptr<Object> res = func1->Applay(func);
                result.push_back(As<Number>(res)->GetValue());
            } else {
                throw RuntimeError("Bad Function Name");
            }
            curr = As<Cell>(curr)->GetSecond();
            continue;
        }
        if (!Is<Number>(first)) {
            if (Is<Symbol>(first)) {
                std::string var_name = first->ToString();
                if (!check_scope) {
                    if (global_scope.find(var_name) != global_scope.end()) {
                        std::shared_ptr<Object> res = global_scope[var_name];
                        result.push_back(As<Number>(res)->GetValue());
                        curr = As<Cell>(curr)->GetSecond();
                        continue;
                    }
                } else {
                    if (lambda_var_scope.find(var_name) != lambda_var_scope.end()) {
                        std::shared_ptr<Object> res = lambda_var_scope[var_name];
                        result.push_back(As<Number>(res)->GetValue());
                        curr = As<Cell>(curr)->GetSecond();
                        continue;
                    }
                }
            }
            throw RuntimeError("Bad element for operator");
        }
        result.push_back(As<Number>(first)->GetValue());
        curr = As<Cell>(curr)->GetSecond();
    }
    for (size_t i = 1; i < result.size(); ++i) {
        if (ComparatorOp(result[i - 1], result[i], op)) {
            continue;
        }
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> EqualFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForOp(obj, "=");
}

std::shared_ptr<Object> MoreFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForOp(obj, ">");
}

std::shared_ptr<Object> LessFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForOp(obj, "<");
}

std::shared_ptr<Object> MoreOrEqualFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForOp(obj, ">=");
}

std::shared_ptr<Object> LessOrEqualFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForOp(obj, "<=");
}

// Function max min

bool ComparatorMM(int64_t& a, int64_t b, const std::string& m) {
    if (m == "max") {
        return a < b;
    } else {
        return a > b;
    }
}

std::shared_ptr<Object> ApplayForMM(std::shared_ptr<Object> obj, const std::string& m) {
    int64_t val;
    if (m == "max") {
        val = std::numeric_limits<int64_t>::min();
    } else {
        val = std::numeric_limits<int64_t>::max();
    }
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (curr == nullptr) {
        throw RuntimeError("Too few args");
    }
    while (curr != nullptr) {
        std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
        if (Is<Cell>(first)) {
            std::shared_ptr<Object> func = As<Cell>(first)->GetFirst();
            if (func == nullptr) {
                throw RuntimeError("Bad syntax");
            }
            std::string name = func->ToString();
            std::shared_ptr<Object> obj_val;
            if (functionMap.find(name) != functionMap.end()) {
                std::shared_ptr<Object> func1 = functionMap[name];
                obj_val = func1->Applay(func);
            } else {
                throw RuntimeError("Bad Function Name");
            }
            if (ComparatorMM(val, As<Number>(obj_val)->GetValue(), m)) {
                val = As<Number>(obj_val)->GetValue();
            }
            curr = As<Cell>(curr)->GetSecond();
            continue;
        }
        if (!Is<Number>(first)) {
            throw RuntimeError("Bad element for operator");
        }
        if (ComparatorMM(val, As<Number>(first)->GetValue(), m)) {
            val = As<Number>(first)->GetValue();
        }
        curr = As<Cell>(curr)->GetSecond();
    }
    return std::make_shared<Number>(val);
}

std::shared_ptr<Object> MaxFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForMM(obj, "max");
}

// Fucntion min
std::shared_ptr<Object> MinFunction::Applay(std::shared_ptr<Object> obj) {
    return ApplayForMM(obj, "min");
}

// my general realisation for + - / *
std::shared_ptr<Object> ApplayForPMMD(std::shared_ptr<Object> obj, const std::string& op) {
    std::vector<int64_t> result;
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (Is<Symbol>(curr) || Is<Number>(curr)) {
        throw RuntimeError("you cant use pair");
    }
    while (curr != nullptr) {
        std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
        std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
        if (Is<Cell>(first)) {
            std::shared_ptr<Object> func = As<Cell>(first)->GetFirst();
            if (func == nullptr) {
                throw RuntimeError("Bad syntax");
            }
            std::string name = func->ToString();
            if (name == "+" || name == "-" || name == "/" || name == "*") {          
                std::shared_ptr<Object> func1 = functionMap[name];
                std::shared_ptr<Object> res = func1->Applay(first);
                result.push_back(As<Number>(res)->GetValue());
            } else {
                throw RuntimeError("Bad Syntax for Math Operations");
            }
            curr = second;
            continue;
        }
        if (!Is<Number>(first)) {
            if (Is<Symbol>(first)) {
                std::string check = first->ToString();
                if (!check_scope) {
                    if (global_scope.find(check) != global_scope.end()) {
                        result.push_back(As<Number>(global_scope[check])->GetValue());
                        curr = second;
                        continue;
                    }
                } else {
                    if (lambda_var_scope.find(check) != lambda_var_scope.end()) {
                        result.push_back(As<Number>(lambda_var_scope[check])->GetValue());
                        curr = second;
                        continue;
                    }
                }
            }
            throw RuntimeError("Bad arg");
        }
        result.push_back(As<Number>(first)->GetValue());
        curr = second;
        if (Is<Symbol>(curr) || Is<Number>(curr)) {
            throw RuntimeError("you cant use pair");
        }
    }
    int64_t ans;
    if (op == "+") {
        ans = 0;
        for (int64_t elem : result) {
            ans += elem;
        }
        return std::make_shared<Number>(ans);
    }
    if (op == "*") {
        ans = 1;
        for (int64_t elem : result) {
            ans *= elem;
        }
        return std::make_shared<Number>(ans);
    }
    if (op == "-") {
        ans = result[0];
        for (size_t i = 1; i < result.size(); ++i) {
            ans -= result[i];
        }
        return std::make_shared<Number>(ans);
    }
    ans = result[0];
    for (size_t i = 1; i < result.size(); ++i) {
        ans /= result[i];
    }
    return std::make_shared<Number>(ans);
}

std::shared_ptr<Object> AddFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        return std::make_shared<Number>(0);
    }
    return ApplayForPMMD(obj, "+");
}

std::shared_ptr<Object> SubFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> check = As<Cell>(obj)->GetSecond();
    if (check == nullptr || Is<Symbol>(check)) {
        throw RuntimeError("Wrong syntax for sub");
    }
    return ApplayForPMMD(obj, "-");
}

std::shared_ptr<Object> MultiplyFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        return std::make_shared<Number>(1);
    }
    return ApplayForPMMD(obj, "*");
}

std::shared_ptr<Object> DivisionFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        throw RuntimeError("Wrong syntax for division");
    }
    return ApplayForPMMD(obj, "/");
}

// boolean? Function
std::shared_ptr<Object> BooleanFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        throw RuntimeError("Empty args");
    }
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (As<Cell>(curr)->GetSecond() != nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    if (Is<Symbol>(first)) {
        std::string name = first->ToString();
        if (name == "#t" || name == "#f") {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#f");
}

// not function
std::shared_ptr<Object> NotFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        throw RuntimeError("Too few args");
    }
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
    if (second != nullptr) {
        throw RuntimeError("Too many args for not");
    }
    if (Is<Symbol>(first)) {
        if (first->ToString() == "#f") {
            return std::make_shared<Symbol>("#t");
        }
    }
    return std::make_shared<Symbol>("#f");
}

//my general realisation for and/or

std::vector<std::shared_ptr<Object>> AndOr(std::shared_ptr<Object> obj) {
    std::vector<std::shared_ptr<Object>> result;
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    while (curr != nullptr) {
        std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
        std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
        if (Is<Cell>(first)) {
            std::shared_ptr<Object> func = As<Cell>(first)->GetFirst();
            if (func == nullptr) {
                throw RuntimeError("Bad Syntax");
            }
            std::string name = func->ToString();
            if (functionMap.find(name) != functionMap.end()) {
                std::shared_ptr<Object> func1 = functionMap[name];
                result.push_back(func1->Applay(first));
            } 
            curr = second;
            continue;
        }
        result.push_back(first);
        curr = second;
    }
    return result;
}

std::shared_ptr<Object> AndFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        return std::make_shared<Symbol>("#t");
    }
    std::vector<std::shared_ptr<Object>> result = AndOr(obj);
    for (auto elem : result) {
        if (Is<Symbol>(elem)) {
            if (elem->ToString() == "#f") {
                return std::make_shared<Symbol>("#f");
            }
        }
    }
    return result.back();
}

std::shared_ptr<Object> OrFunction::Applay(std::shared_ptr<Object> obj) {
    if (As<Cell>(obj)->GetSecond() == nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    std::vector<std::shared_ptr<Object>> result = AndOr(obj);
    if (result.back()->ToString() != "#f") {
        return result.back();
    }
    for (auto elem : result) {
        if (Is<Symbol>(elem)) {
            if (elem->ToString() == "#f") {
                continue;
            }
        }
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");

}

// null? function
std::shared_ptr<Object> NullFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (curr == nullptr) {
        return std::make_shared<Symbol>("#t");
    }
    if (!Is<Cell>(curr)) {
        return std::make_shared<Symbol>("#f");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() != "quote") {
        return std::make_shared<Symbol>("#f");
    }
    QuoteFunction func;
    std::shared_ptr<Object> res = func.Applay(first);
    if (res == nullptr) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

// pair? function
std::shared_ptr<Object> PairFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (curr == nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() == "quote") {
        QuoteFunction func;
        std::shared_ptr<Object> res = func.Applay(first);
        if (!Is<Cell>(res)) {
            return std::make_shared<Symbol>("#f");
        }
        std::shared_ptr<Object> second = As<Cell>(res)->GetSecond();
        if (Is<Symbol>(second) || Is<Number>(second)) {
            return std::make_shared<Symbol>("#t");
        }
        if (second == nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
        if (check != nullptr) {
            return std::make_shared<Symbol>("#f");
        }
        return std::make_shared<Symbol>("#t");
    }
    throw RuntimeError("Bad args for pair?");
}

// list? function
std::shared_ptr<Object> ListFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (curr == nullptr) {
        return std::make_shared<Symbol>("#f");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() == "quote") {
        QuoteFunction func;
        std::shared_ptr<Object> res = func.Applay(first);
        if (res == nullptr) {
            return std::make_shared<Symbol>("#t");
        }
        if (Is<Cell>(res)) {  // need to check that it is not function
            std::shared_ptr<Object> second = As<Cell>(res)->GetSecond();
            while (second != nullptr) {
                if (Is<Number>(second) || Is<Symbol>(second)) {
                    return std::make_shared<Symbol>("#f");
                }
                second = As<Cell>(second)->GetSecond();
            }
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
    throw RuntimeError("Bad args for list?");
}

// function cons
std::shared_ptr<Object> ConsFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (!Is<Cell>(curr)) {
        throw RuntimeError("Bad args for cons");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
    if (!Is<Cell>(second)) {
        throw RuntimeError("Bad args for cons");
    }
    std::shared_ptr<Object> next_first = As<Cell>(second)->GetFirst();
    std::shared_ptr<Object> next_second = As<Cell>(second)->GetSecond();
    if (next_second != nullptr) {
        throw RuntimeError("Bad args for cons");
    }
    return std::make_shared<Cell>(first, next_first);
}

// my general realisation for car/cdr

std::shared_ptr<Object> CarCdr(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    if (Is<Symbol>(first)) {
        std::string name = first->ToString();
        if (global_scope.find(name) != global_scope.end()) {
            std::shared_ptr func = global_scope[name];
            return func;
        }
    }
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() == "quote") {
        QuoteFunction func;
        std::shared_ptr<Object> res = func.Applay(first);
        if (res == nullptr) {
            throw RuntimeError("Bad args for car/cdr");
        }
        return res;
    }

    throw RuntimeError("Bad args for car/cdr");
}

std::shared_ptr<Object> CarFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    if (Is<Cell>(first)) {
        while (Is<Cell>(first)) {
            std::shared_ptr<Object> check = As<Cell>(first)->GetFirst();
            if (check->ToString() != "cdr") {
                return As<Cell>(CarCdr(obj))->GetFirst();
            }
            first = As<Cell>(first)->GetSecond();

            first = As<Cell>(first)->GetFirst();
        }
        std::string name = first->ToString();
        std::shared_ptr res = global_scope[name];
        std::shared_ptr<Object> first = As<Cell>(res)->GetSecond();
        return As<Cell>(first)->GetFirst();
    }

    return As<Cell>(CarCdr(obj))->GetFirst();
}

std::shared_ptr<Object> CdrFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    if (Is<Cell>(first)) {
        while (Is<Cell>(first)) {
            std::shared_ptr<Object> check = As<Cell>(first)->GetFirst();
            if (check->ToString() != "car") {
                return As<Cell>(CarCdr(obj))->GetSecond();
            }
            first = As<Cell>(first)->GetSecond();
            first = As<Cell>(first)->GetFirst();
        }
        std::string name = first->ToString();
        std::shared_ptr res = global_scope[name];
        std::shared_ptr<Object> first = As<Cell>(res)->GetFirst();
        return As<Cell>(first)->GetSecond();
    }
    return As<Cell>(CarCdr(obj))->GetSecond();
}

// function list
std::shared_ptr<Object> MakeListFunction::Applay(std::shared_ptr<Object> obj) {
    return As<Cell>(obj)->GetSecond();
}

// function list-tail
std::shared_ptr<Object> ListTailFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
    std::shared_ptr<Object> ast_ind = As<Cell>(second)->GetFirst();
    int64_t index = As<Number>(ast_ind)->GetValue();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() == "quote") {
        QuoteFunction func;
        std::shared_ptr<Object> res = func.Applay(first);
        while (index != 0) {
            if (res == nullptr) {
                throw RuntimeError("bad index for list");
            }
            res = As<Cell>(res)->GetSecond();
            index--;
        }
        return res;
    }
    throw RuntimeError("Bad args for list-tail");
}

// function list-ref
std::shared_ptr<Object> ListRefFunction::Applay(std::shared_ptr<Object> obj) {
    ListTailFunction func;
    std::shared_ptr<Object> curr = func.Applay(obj);
    if (curr == nullptr) {
        throw RuntimeError("Bad Index For list-ref");
    }
    return As<Cell>(curr)->GetFirst();
}

//symbol? function
std::shared_ptr<Object> SymbolFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    if (Is<Number>(first)) {
        return std::make_shared<Symbol>("#f");
    }
    if (Is<Symbol>(first)) {
        return std::make_shared<Symbol>("#t");
    }
    std::shared_ptr<Object> name = As<Cell>(first)->GetFirst();
    if (name->ToString() == "quote") {
        QuoteFunction func;
        std::shared_ptr<Object> res = func.Applay(first);
        if (Is<Symbol>(res)) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#f");
}

//define function
std::shared_ptr<Object> DefineFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (!Is<Cell>(curr)) {
        throw SyntaxError("Bad Syntax for define");
    }
    std::shared_ptr<Object> var = As<Cell>(curr)->GetFirst();
    if (!Is<Symbol>(var)) {
        throw SyntaxError("Bad Syntax for define");
    }
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
    if (!Is<Cell>(second)) {
        throw SyntaxError("Bad Syntax for define");
    }
    std::shared_ptr<Object> first = As<Cell>(second)->GetFirst();
    if (Is<Symbol>(first)) {
        std::string check = first->ToString();
        if (global_scope.find(check) != global_scope.end()) {
            global_scope[var->ToString()] = global_scope[check];
            return global_scope[check];
        }
        throw NameError("Bad Syntax for define");
    }
    std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
    if (Is<Cell>(first) && check == nullptr) {
        std::shared_ptr<Object> func_name = As<Cell>(first)->GetFirst();
        std::string name = func_name->ToString();
        if (name == "lambda") {
            scope_for_lambda[var->ToString()] = first;
            return first;
        }
        if (functionMap.find(name) != functionMap.end()) {
            std::shared_ptr func = functionMap[name];
            std::shared_ptr<Object> result = func->Applay(first);         
            global_scope[var->ToString()] = result;
            return result;
        } 
        throw SyntaxError("Bad Syntax for define");
    }
    if (Is<Number>(first) && check == nullptr) {
        global_scope[var->ToString()] = first;
        return first;
    }
    throw SyntaxError("Bad Syntax for define");
}

//set! function
std::shared_ptr<Object> SetFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (!Is<Cell>(curr)) {
        throw SyntaxError("Bad Syntax for set!");
    }
    std::shared_ptr<Object> var = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();
    if (!Is<Symbol>(var)) {
        throw SyntaxError("Bad Syntax for set!");
    }
    if (!check_scope) {
        if (global_scope.find(var->ToString()) == global_scope.end()) {    
            if (Is<Cell>(second)) {
                std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
                if (Is<Cell>(check)) {
                    throw SyntaxError("bad syntax for set!");
                }
            }
            throw NameError("Not declared for set!");
        }
    } else {
        if (lambda_var_scope.find(var->ToString()) == lambda_var_scope.end()) {    
            if (Is<Cell>(second)) {
                std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
                if (Is<Cell>(check)) {
                    throw SyntaxError("bad syntax for set!");
                }
            }
            throw NameError("Not declared for set!");
        }
    }
    
    if (!Is<Cell>(second)) {
        throw SyntaxError("Bad Syntax for set!");
    }
    std::shared_ptr<Object> first = As<Cell>(second)->GetFirst();
    std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
    if (Is<Cell>(first) && check == nullptr) {
        std::shared_ptr<Object> func_name = As<Cell>(first)->GetFirst();
        std::string name = func_name->ToString();
        if (functionMap.find(name) != functionMap.end()) {
            std::shared_ptr func = functionMap[name];
            std::shared_ptr<Object> result = func->Applay(first);
            if (!Is<Number>(result)) {

                throw SyntaxError("Bad Syntax for set!");
            }
            if (!check_scope) {
                global_scope[var->ToString()] = result;
            } else {
                lambda_var_scope[var->ToString()] = result;
            }
            return result;
        } 
      
        throw SyntaxError("Bad Syntax for set!");
    }
    if (Is<Number>(first) && check == nullptr) {
        if (!check_scope) {
            global_scope[var->ToString()] = first;
        } else {
            lambda_var_scope[var->ToString()] = first;
        };
        return first;
    }
    throw SyntaxError("Bad Syntax for set!");
}

//if function
std::shared_ptr<Object> IfFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    if (!Is<Cell>(curr)) {
        throw SyntaxError("Bad Syntax for if1");
    }
    std::shared_ptr<Object> first = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(curr)->GetSecond();

    std::shared_ptr<Object> condition;
    if (Is<Cell>(first)) {
        std::shared_ptr<Object> func_name = As<Cell>(first)->GetFirst();
        std::string name = func_name->ToString();
        if (functionMap.find(name) != functionMap.end()) {
            std::shared_ptr func = functionMap[name];
            condition = func->Applay(first);
        } else {
            throw SyntaxError("Bad Syntax for if2");
        }    
    } else {
        condition = first;
    }
    if (condition->ToString() == "#f") {
        std::shared_ptr<Object> check = As<Cell>(second)->GetSecond();
        if (check == nullptr) {
            return nullptr;
        }
        std::shared_ptr<Object> result = As<Cell>(check)->GetFirst();
        if (Is<Cell>(result)) {
            std::shared_ptr<Object> func_name = As<Cell>(result)->GetFirst();
            std::string name = func_name->ToString();
            if (functionMap.find(name) != functionMap.end()) {
                std::shared_ptr func = functionMap[name];
                return func->Applay(result);
            }
            if (scope_for_lambda.find(name) != scope_for_lambda.end()) {
                std::shared_ptr<Object> second = As<Cell>(result)->GetSecond();
                while (second != nullptr) {
                    std::shared_ptr<Object> var = As<Cell>(second)->GetFirst();
                    if (Is<Cell>(var)) {
                        std::shared_ptr func_name = As<Cell>(var)->GetFirst();
                        std::string name = func_name->ToString();
                        if (functionMap.find(name) != functionMap.end()) {
                            std::shared_ptr func = functionMap[name];
                            std::shared_ptr<Object> res = func->Applay(var);
                            lambda_var.push_back(As<Number>(res)->GetValue());
                            std::cout << lambda_var.back() << " ";
                            second = As<Cell>(second)->GetSecond();
                            continue;
                        }                  
                    }
                    lambda_var.push_back(As<Number>(var)->GetValue());
                    second = As<Cell>(second)->GetSecond();
                }
                std::shared_ptr<Object> curr_lambda = scope_for_lambda[name];
                LambdaFunction func;
                std::shared_ptr<Object> res = func.Applay(curr_lambda);
                return res;
            }
            throw SyntaxError("Bad Syntax for if3");
            
        }
        return result;
    }

    std::shared_ptr<Object> result = As<Cell>(second)->GetFirst();
    if (Is<Symbol>(result)) {
        std::string var_name = result->ToString();
        if (lambda_var_scope.find(var_name) != lambda_var_scope.end()) {
            return lambda_var_scope[var_name];
        }
    }
    std::shared_ptr<Object> errors_check = As<Cell>(second)->GetSecond();
    if (Is<Cell>(errors_check)) {
        std::shared_ptr<Object> final_check =  As<Cell>(errors_check)->GetSecond();
        if (final_check != nullptr) {
            throw SyntaxError("Bad Syntax for if4");
        }
    }
    if (Is<Cell>(result)) {
        std::shared_ptr<Object> func_name = As<Cell>(result)->GetFirst();
        std::string name = func_name->ToString();
        if (functionMap.find(name) != functionMap.end()) {
            std::shared_ptr func = functionMap[name];
            return func->Applay(result);
        } else {
            throw SyntaxError("Bad Syntax for if5");
        }    
    }
    return result;
}

//set-car! function
std::shared_ptr<Object> SetCarFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> var = As<Cell>(curr)->GetFirst();
    std::string name = var->ToString();
    std::shared_ptr<Object> val = As<Cell>(curr)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(val)->GetFirst();
    if (Is<Number>(first)) {
        std::shared_ptr res = global_scope[name];
        std::shared_ptr<Object> save = As<Cell>(res)->GetSecond();
        global_scope[name] = std::make_shared<Cell>(first, save);
        return global_scope[name];
    }
    
    std::string change_var = first->ToString();
    std::shared_ptr res = global_scope[name];
    std::shared_ptr change = global_scope[change_var];
    std::shared_ptr<Object> copy = res;
    std::shared_ptr<Object> second = As<Cell>(copy)->GetSecond();
    global_scope[name] = std::make_shared<Cell>(change, second);
    return std::make_shared<Cell>(change, second);
}

//set-cdr! function 
std::shared_ptr<Object> SetCdrFunction::Applay(std::shared_ptr<Object> obj) {
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> var = As<Cell>(curr)->GetFirst();
    std::string name = var->ToString();
    std::shared_ptr<Object> val = As<Cell>(curr)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(val)->GetFirst();
    if (Is<Number>(first)) {
        std::shared_ptr res = global_scope[name];
        std::shared_ptr<Object> save = As<Cell>(res)->GetFirst();
        global_scope[name] = std::make_shared<Cell>(save, first);
        return global_scope[name];
    }
    std::string change_var = first->ToString();
    std::shared_ptr res = global_scope[name];
    std::shared_ptr change = global_scope[change_var];
    std::shared_ptr<Object> copy = res;
    std::shared_ptr<Object> second = As<Cell>(copy)->GetFirst();
    global_scope[name] = std::make_shared<Cell>(second, change);
    return std::make_shared<Cell>(second, change);
}
/* not ready
//lambda function
std::shared_ptr<Object> LambdaFunction::Applay(std::shared_ptr<Object> obj) {
    check_scope = true;
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetSecond();
    std::shared_ptr<Object> reading_vars = As<Cell>(curr)->GetFirst();
    std::shared_ptr<Object> var = As<Cell>(reading_vars)->GetFirst();
    std::shared_ptr<Object> check = As<Cell>(reading_vars)->GetSecond();
    std::string name_var = var->ToString();
    lambda_var_scope[name_var] = std::make_shared<Number>(lambda_var.front());
    lambda_var.pop_front();
    while (check != nullptr) {
        var = As<Cell>(check)->GetFirst();
        name_var = var->ToString();
        lambda_var_scope[name_var] = std::make_shared<Number>(lambda_var.front());
        lambda_var.pop_front();
        check = As<Cell>(check)->GetSecond();
    }
    std::shared_ptr<Object> body_lambda = As<Cell>(curr)->GetSecond();
    std::shared_ptr<Object> first = As<Cell>(body_lambda)->GetFirst();
    std::shared_ptr<Object> func = As<Cell>(first)->GetFirst();
    std::string func_name = func->ToString();
    
    if (functionMap.find(func_name) != functionMap.end()) {
        func = functionMap[func_name];
        std::shared_ptr<Object> res= func->Applay(first);
        if (func_name == "if") {
            check_scope = false;
            return res;
        }
    }
    std::shared_ptr<Object> ret = As<Cell>(body_lambda)->GetSecond();
    std::shared_ptr<Object> body_ret = As<Cell>(ret)->GetFirst();
    first = As<Cell>(body_ret)->GetFirst();
    func_name = first->ToString();
    if (functionMap.find(func_name) != functionMap.end()) {
        func = functionMap[func_name];
        std::shared_ptr<Object> res = func->Applay(body_ret);
        check_scope = false;
        return res;
    }
}
*/
////////////////////////////////////////////////////

std::string Interpreter::Run(const std::string& in) {
    std::stringstream ss{in};
    Tokenizer tokenizer{&ss};
    std::shared_ptr<Object> obj = Read(&tokenizer);
    if (Is<Symbol>(obj)) {
        if (global_scope.find(obj->ToString()) != global_scope.end()) {
            std::shared_ptr<Object> res = global_scope[obj->ToString()];
            return res->ToString();
        }   
        if (obj->ToString() == "#t" || obj->ToString() == "#f") {
            return obj->ToString();
        }
        throw NameError("Bad Syntax");
    }
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("bad string");
    }
    if (obj == nullptr) {
        throw RuntimeError("bad string");
    }
    if (Is<Number>(obj)) {
        return obj->ToString();
    }
    std::shared_ptr<Object> curr = As<Cell>(obj)->GetFirst();
    if (curr == nullptr || Is<Cell>(curr) || obj == nullptr) {
        throw RuntimeError("bad string");
    }
    if (!Is<Symbol>(curr)) {
        if (Is<Number>(curr)) {
            throw RuntimeError("bad string");
        }
        throw SyntaxError("bad string");
    }
    std::string func_name = As<Symbol>(curr)->GetName();
    if (functionMap.find(func_name) != functionMap.end()) {
        std::shared_ptr func = functionMap[func_name];
        std::shared_ptr<Object> result = func->Applay(obj);
        if (result == nullptr) {
            return "()";
        }
        return result->ToString();
    } 
    if (scope_for_lambda.find(func_name) != scope_for_lambda.end()) {
        std::shared_ptr<Object> second = As<Cell>(obj)->GetSecond();
        while (second != nullptr) {
            std::shared_ptr<Object> var = As<Cell>(second)->GetFirst();
            lambda_var.push_back(As<Number>(var)->GetValue());
            second = As<Cell>(second)->GetSecond();
        }
        std::shared_ptr<Object> curr_lambda = scope_for_lambda[func_name];
        LambdaFunction func;
        std::shared_ptr<Object> res = func.Applay(curr_lambda);
        return res->ToString();
    }

    throw RuntimeError("Bad string");
}
