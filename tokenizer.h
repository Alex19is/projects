#pragma once

#include "error.h"
#include "bits/stdc++.h"

const std::string kAllSymbStart = "abcdefghijklmnopqrstuvwxyzABCDEFGHIGKLMNOPQRSTUVWXYZ<=>*/#+-";
const std::string kAllSymbCont = kAllSymbStart + "0123456789?!-";

struct SymbolToken {
    std::string name;

    bool operator==(const SymbolToken& other) const {
        if (name == other.name) {
            return true;
        }
        return false;
    }
};

struct None {  // struct(flag) for IsEnd Tokenizer method;
    bool operator==(const None&) const {
        return true;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;

    bool operator==(const ConstantToken& other) const {
        if (value == other.value) {
            return true;
        }
        return false;
    }
};

using Token = std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, None>;

bool IsNumber(const std::string& str);

void Initialisation(Token& token, const std::string& str);

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

private:
    Token token_;
    std::istream* input_;
    std::string read_;  // using for token_ Initialisation
};
