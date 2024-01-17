#include <tokenizer.h>

bool IsNumber(const std::string& str) {
    if (str.empty() || str == "-") {
        return false;
    }
    if (!isdigit(str[0])) {
        if (str[0] != '-') {
            return false;
        }
    }
    for (int i = 1; i < str.size(); ++i) {
        if (isdigit(str[i])) {
            continue;
        }
        return false;
    }
    return true;
}

void Initialisation(Token& token, const std::string& str) {
    switch (str[0]) {
        case '(':
            token = Token{BracketToken::OPEN};
            break;
        case ')':
            token = Token{BracketToken::CLOSE};
            break;
        case '.':
            token = Token{DotToken{}};
            break;
        case '\'':
            token = Token{QuoteToken{}};
            break;
        default:
            if (IsNumber(str)) {
                int val = std::stoi(str);
                token = Token{ConstantToken{val}};
            } else {
                if (kAllSymbStart.find(str[0]) == std::string::npos) {
                    throw SyntaxError("Invalid First Symbol in Token");
                }
                for (int i = 1; i < str.size(); ++i) {
                    if (kAllSymbCont.find(str[i]) == std::string::npos) {
                        throw SyntaxError("Invalid Middle Symbole in Token");
                    }
                }
                token = Token{SymbolToken{str}};
            }
            break;
    }
}

Tokenizer::Tokenizer(std::istream* in) : input_(in) {
    Next();
}

void Tokenizer::Next() {
    token_ = Token{None{}};
    while (true) {                    // i dont know how write it good
        if (input_->peek() == EOF) {  // end of line;
            if (read_.empty()) {
                break;
            }
            Initialisation(token_, read_);
            read_.clear();
            break;
        }
        char c = input_->get();
        if (c == ' ' || c == '\n') {
            if (read_.empty()) {
                continue;
            }
            Initialisation(token_, read_);
            read_.clear();
            break;
        }
        if (c == '(' || c == ')' || c == '.' || c == '\'') {
            if (!read_.empty()) {
                Initialisation(token_, read_);
                read_.clear();
                read_ += c;
                break;
            }
            read_ += c;
            Initialisation(token_, read_);
            read_.clear();
            break;
        }
        if (!read_.empty()) {
            if (read_ == "+" || read_ == "-") {
                if (isdigit(c)) {
                    if (read_ == "+") {
                        read_.clear();
                        read_ += c;
                    } else {
                        read_ += c;
                    }
                }

                continue;
            }
            if (c == '+') {
                Initialisation(token_, read_);
                read_.clear();
                read_ += c;
                break;
            }
            if (c == '-') {
                read_ += c;
                continue;
            }
            read_ += c;

        } else {
            read_ += c;
        }
    }
}

bool Tokenizer::IsEnd() {
    if (token_ == Token{None{}}) {
        return true;
    }
    return false;
}

Token Tokenizer::GetToken() {
    return token_;
}
