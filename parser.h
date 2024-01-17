#pragma once

#include <memory>

#include "object.h"
#include <tokenizer.h>

inline std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);

inline std::shared_ptr<Object> Read(Tokenizer* tokenizer) {

    if (std::holds_alternative<ConstantToken>(tokenizer->GetToken())) {
        int curr_num = std::get<ConstantToken>(tokenizer->GetToken()).value;
        tokenizer->Next();
        return std::make_shared<Number>(curr_num);
    }
    if (std::holds_alternative<SymbolToken>(tokenizer->GetToken())) {
        std::string curr_str = std::get<SymbolToken>(tokenizer->GetToken()).name;
        tokenizer->Next();
        return std::make_shared<Symbol>(curr_str);
    }
    if (tokenizer->GetToken() == Token{BracketToken::OPEN}) {
        tokenizer->Next();
        return ReadList(tokenizer);
    }
    if (tokenizer->GetToken() == Token{QuoteToken{}}) {
        tokenizer->Next();
        std::shared_ptr<Object> first = std::make_shared<Symbol>("quote");
        std::shared_ptr<Object> first1 = Read(tokenizer);
        std::shared_ptr<Object> second1 = nullptr;
        std::shared_ptr<Object> second = std::make_shared<Cell>(first1, second1);
        return std::make_shared<Cell>(first, second);
    }
    throw SyntaxError("Bad Syntax");
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        tokenizer->Next();
        return nullptr;
    } else {
        std::shared_ptr<Object> first = Read(tokenizer);
        if (tokenizer->GetToken() == Token{DotToken{}}) {
            tokenizer->Next();
            std::shared_ptr<Object> second = Read(tokenizer);
            if (tokenizer->GetToken() != Token{BracketToken::CLOSE}) {
                throw SyntaxError("Bad Syntax");
            }
            tokenizer->Next();
            return std::make_shared<Cell>(first, second);
        } else {
            std::shared_ptr<Object> second = ReadList(tokenizer);
            return std::make_shared<Cell>(first, second);
        }
    }
}
