#pragma once

#include <charconv>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace jsonpp
{

enum class TokenType {
    STRING,
    NUMBER,
    BOOLEAN_TRUE,
    BOOLEAN_FALSE,
    NULL_VALUE,
    OBJECT_START, // {
    OBJECT_END,   // }
    ARRAY_START,  // [
    ARRAY_END,    // ]
    COLON,        // :
    COMMA,        // ,
    END_OF_INPUT
};

struct Token {
    TokenType type;
    std::string_view value; // Points into the original input buffer
    size_t line = 1;
    size_t column = 1;

    constexpr Token(TokenType t, std::string_view v = {}, size_t l = 1,
                    size_t c = 1)
        : type(t), value(v), line(l), column(c)
    {
    }
};

class TokenizerError : public std::runtime_error
{
  public:
    TokenizerError(const std::string &message, size_t line, size_t column)
        : std::runtime_error(message), line_(line), column_(column)
    {
    }

    size_t line() const noexcept { return line_; }
    size_t column() const noexcept { return column_; }

  private:
    size_t line_;
    size_t column_;
};

class Tokenizer
{
  public:
    constexpr explicit Tokenizer(std::string_view input)
        : input_(input), pos_(0), line_(1), column_(1)
    {
    }

    // Get next token from input stream
    constexpr Token next_token()
    {
        skip_whitespace();

        if (pos_ >= input_.size()) {
            return Token{TokenType::END_OF_INPUT, {}, line_, column_};
        }

        char current = input_[pos_];
        size_t token_line = line_;
        size_t token_column = column_;

        switch (current) {
        case '{':
            advance();
            return Token{TokenType::OBJECT_START, {}, token_line, token_column};
        case '}':
            advance();
            return Token{TokenType::OBJECT_END, {}, token_line, token_column};
        case '[':
            advance();
            return Token{TokenType::ARRAY_START, {}, token_line, token_column};
        case ']':
            advance();
            return Token{TokenType::ARRAY_END, {}, token_line, token_column};
        case ':':
            advance();
            return Token{TokenType::COLON, {}, token_line, token_column};
        case ',':
            advance();
            return Token{TokenType::COMMA, {}, token_line, token_column};
        case '"':
            return parse_string(token_line, token_column);
        case 't':
            return parse_literal("true", TokenType::BOOLEAN_TRUE, token_line,
                                 token_column);
        case 'f':
            return parse_literal("false", TokenType::BOOLEAN_FALSE, token_line,
                                 token_column);
        case 'n':
            return parse_literal("null", TokenType::NULL_VALUE, token_line,
                                 token_column);
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return parse_number(token_line, token_column);
        default:
            throw TokenizerError("Unexpected character: " +
                                     std::string(1, current),
                                 line_, column_);
        }
    }

    // Peek at current position without advancing
    constexpr char peek() const
    {
        return pos_ < input_.size() ? input_[pos_] : '\0';
    }

    // Get current position for error reporting
    constexpr size_t line() const { return line_; }
    constexpr size_t column() const { return column_; }
    constexpr size_t position() const { return pos_; }

  private:
    std::string_view input_;
    size_t pos_;
    size_t line_;
    size_t column_;

    constexpr void advance()
    {
        if (pos_ < input_.size()) {
            if (input_[pos_] == '\n') {
                line_++;
                column_ = 1;
            } else {
                column_++;
            }
            pos_++;
        }
    }

    constexpr void skip_whitespace()
    {
        while (pos_ < input_.size() && is_whitespace(input_[pos_])) {
            advance();
        }
    }

    constexpr bool is_whitespace(char c) const
    {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }

    constexpr Token parse_string(size_t token_line, size_t token_column)
    {
        size_t start_pos = pos_;
        advance(); // Skip opening quote

        while (pos_ < input_.size() && input_[pos_] != '"') {
            if (input_[pos_] == '\\') {
                advance(); // Skip escape character
                if (pos_ >= input_.size()) {
                    throw TokenizerError("Unterminated string escape", line_,
                                         column_);
                }
                advance(); // Skip escaped character
            } else {
                advance();
            }
        }

        if (pos_ >= input_.size()) {
            throw TokenizerError("Unterminated string", line_, column_);
        }

        advance(); // Skip closing quote

        // Return string content without quotes
        auto content = input_.substr(start_pos + 1, pos_ - start_pos - 2);
        return Token{TokenType::STRING, content, token_line, token_column};
    }

    constexpr Token parse_literal(std::string_view literal, TokenType type,
                                  size_t token_line, size_t token_column)
    {
        size_t start_pos = pos_;

        for (char expected : literal) {
            if (pos_ >= input_.size() || input_[pos_] != expected) {
                throw TokenizerError("Invalid literal", line_, column_);
            }
            advance();
        }

        auto token_value = input_.substr(start_pos, literal.size());
        return Token{type, token_value, token_line, token_column};
    }

    constexpr Token parse_number(size_t token_line, size_t token_column)
    {
        size_t start_pos = pos_;

        // Handle optional minus sign
        if (input_[pos_] == '-') {
            advance();
        }

        // Must have at least one digit
        if (pos_ >= input_.size() || !is_digit(input_[pos_])) {
            throw TokenizerError("Invalid number format", line_, column_);
        }

        // Handle integer part
        if (input_[pos_] == '0') {
            advance();
        } else {
            while (pos_ < input_.size() && is_digit(input_[pos_])) {
                advance();
            }
        }

        // Handle optional fractional part
        if (pos_ < input_.size() && input_[pos_] == '.') {
            advance();
            if (pos_ >= input_.size() || !is_digit(input_[pos_])) {
                throw TokenizerError(
                    "Invalid number format: missing fractional digits", line_,
                    column_);
            }
            while (pos_ < input_.size() && is_digit(input_[pos_])) {
                advance();
            }
        }

        // Handle optional exponent part
        if (pos_ < input_.size() &&
            (input_[pos_] == 'e' || input_[pos_] == 'E')) {
            advance();
            if (pos_ < input_.size() &&
                (input_[pos_] == '+' || input_[pos_] == '-')) {
                advance();
            }
            if (pos_ >= input_.size() || !is_digit(input_[pos_])) {
                throw TokenizerError(
                    "Invalid number format: missing exponent digits", line_,
                    column_);
            }
            while (pos_ < input_.size() && is_digit(input_[pos_])) {
                advance();
            }
        }

        auto number_str = input_.substr(start_pos, pos_ - start_pos);
        return Token{TokenType::NUMBER, number_str, token_line, token_column};
    }

    constexpr bool is_digit(char c) const { return c >= '0' && c <= '9'; }
};

inline double extract_number(const Token &token)
{
    if (token.type != TokenType::NUMBER) {
        throw std::invalid_argument("Token is not a number");
    }

    double result = 0.0;
    auto [ptr, ec] = std::from_chars(
        token.value.data(), token.value.data() + token.value.size(), result);

    if (ec != std::errc{}) {
        throw std::invalid_argument("Failed to parse number");
    }

    return result;
}

constexpr bool extract_boolean(const Token &token)
{
    switch (token.type) {
    case TokenType::BOOLEAN_TRUE:
        return true;
    case TokenType::BOOLEAN_FALSE:
        return false;
    default:
        throw std::invalid_argument("Token is not a boolean");
    }
}

constexpr std::string_view extract_string(const Token &token)
{
    if (token.type != TokenType::STRING) {
        throw std::invalid_argument("Token is not a string");
    }
    return token.value;
}

} // namespace jsonpp