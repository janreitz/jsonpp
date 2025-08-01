#include <jsonpp/tokenizer.hpp>
#include <string_view>

using namespace jsonpp;

// Test JSON data as string literal for compile-time evaluation
constexpr std::string_view complex_test_json = R"({
  "user": {
    "id": 12345,
    "name": "Alice Johnson",
    "email": "alice@example.com",
    "active": true,
    "metadata": {
      "created_at": "2023-01-15T10:30:00Z",
      "last_login": "2024-07-30T14:22:33Z",
      "login_count": 847
    },
    "tags": ["admin", "power-user", "beta-tester"],
    "preferences": {
      "theme": "dark",
      "notifications": true,
      "language": "en-US"
    }
  },
  "posts": [
    {
      "id": 101,
      "title": "Getting Started with JSON Parsing",
      "content": "This is a comprehensive guide to JSON parsing...",
      "published": true,
      "views": 1250,
      "tags": ["tutorial", "json", "programming"]
    },
    {
      "id": 102,
      "title": "Advanced Parsing Techniques", 
      "content": "In this post we'll explore more advanced concepts...",
      "published": false,
      "views": 0,
      "tags": ["advanced", "parsing", "performance"]
    }
  ],
  "statistics": {
    "total_users": 50000,
    "active_users": 12500,
    "growth_rate": 0.125,
    "server_uptime": null
  }
})";

// Simple JSON snippets for focused testing
constexpr std::string_view simple_string = R"("hello world")";
constexpr std::string_view simple_number = "42.5";
constexpr std::string_view simple_true = "true";
constexpr std::string_view simple_false = "false";
constexpr std::string_view simple_null = "null";
constexpr std::string_view simple_object = R"({"key": "value"})";
constexpr std::string_view simple_array = R"([1, 2, 3])";

// ============================================================================
// COMPILE-TIME TESTS (static_assert)
// ============================================================================

// Basic tokenizer construction
constexpr bool test_tokenizer_construction()
{
    Tokenizer tokenizer(simple_string);
    return true; // If this compiles, construction is constexpr
}

// String tokenization
constexpr bool test_string_tokenization()
{
    Tokenizer tokenizer(simple_string);
    Token token = tokenizer.next_token();

    return token.type == TokenType::STRING && token.value == "hello world" &&
           token.line == 1 && token.column == 1 && token.is_valid();
}

// Number tokenization
constexpr bool test_number_tokenization()
{
    Tokenizer tokenizer(simple_number);
    Token token = tokenizer.next_token();

    return token.type == TokenType::NUMBER && token.value == "42.5" &&
           token.is_valid();
}

// Boolean tokenization
constexpr bool test_boolean_tokenization()
{
    Tokenizer true_tokenizer(simple_true);
    Tokenizer false_tokenizer(simple_false);

    Token true_token = true_tokenizer.next_token();
    Token false_token = false_tokenizer.next_token();

    return true_token.type == TokenType::BOOLEAN_TRUE &&
           true_token.value == "true" && true_token.is_valid() &&
           false_token.type == TokenType::BOOLEAN_FALSE &&
           false_token.value == "false" && false_token.is_valid();
}

// Null tokenization
constexpr bool test_null_tokenization()
{
    Tokenizer tokenizer(simple_null);
    Token token = tokenizer.next_token();

    return token.type == TokenType::NULL_VALUE && token.value == "null" &&
           token.is_valid();
}

// Object structure parsing
constexpr bool test_object_structure()
{
    Tokenizer tokenizer(simple_object);

    Token token1 = tokenizer.next_token(); // {
    Token token2 = tokenizer.next_token(); // "key"
    Token token3 = tokenizer.next_token(); // :
    Token token4 = tokenizer.next_token(); // "value"
    Token token5 = tokenizer.next_token(); // }
    Token token6 = tokenizer.next_token(); // EOF

    return token1.type == TokenType::OBJECT_START &&
           token2.type == TokenType::STRING && token2.value == "key" &&
           token3.type == TokenType::COLON &&
           token4.type == TokenType::STRING && token4.value == "value" &&
           token5.type == TokenType::OBJECT_END &&
           token6.type == TokenType::END_OF_INPUT && token1.is_valid() &&
           token2.is_valid() && token3.is_valid() && token4.is_valid() &&
           token5.is_valid() && token6.is_valid();
}

// Array structure parsing
constexpr bool test_array_structure()
{
    Tokenizer tokenizer(simple_array);

    Token token1 = tokenizer.next_token(); // [
    Token token2 = tokenizer.next_token(); // 1
    Token token3 = tokenizer.next_token(); // ,
    Token token4 = tokenizer.next_token(); // 2
    Token token5 = tokenizer.next_token(); // ,
    Token token6 = tokenizer.next_token(); // 3
    Token token7 = tokenizer.next_token(); // ]
    Token token8 = tokenizer.next_token(); // EOF

    return token1.type == TokenType::ARRAY_START &&
           token2.type == TokenType::NUMBER && token2.value == "1" &&
           token3.type == TokenType::COMMA &&
           token4.type == TokenType::NUMBER && token4.value == "2" &&
           token5.type == TokenType::COMMA &&
           token6.type == TokenType::NUMBER && token6.value == "3" &&
           token7.type == TokenType::ARRAY_END &&
           token8.type == TokenType::END_OF_INPUT && token2.is_valid() &&
           token4.is_valid() && token6.is_valid();
}

// Whitespace handling
constexpr bool test_whitespace_handling()
{
    constexpr std::string_view spaced_json = "  \t\n  {  \r\n  }  ";
    Tokenizer tokenizer(spaced_json);

    Token token1 = tokenizer.next_token(); // {
    Token token2 = tokenizer.next_token(); // }
    Token token3 = tokenizer.next_token(); // EOF

    return token1.type == TokenType::OBJECT_START &&
           token2.type == TokenType::OBJECT_END &&
           token3.type == TokenType::END_OF_INPUT && token1.is_valid() &&
           token2.is_valid() && token3.is_valid();
}

// Position tracking
constexpr bool test_position_tracking()
{
    constexpr std::string_view multiline_json = R"({
  "key": "value"
})";

    Tokenizer tokenizer(multiline_json);

    Token token1 = tokenizer.next_token(); // { at 1:1
    Token token2 = tokenizer.next_token(); // "key" at 2:3
    Token token3 = tokenizer.next_token(); // : at 2:8
    Token token4 = tokenizer.next_token(); // "value" at 2:10
    Token token5 = tokenizer.next_token(); // } at 3:1

    return token1.line == 1 && token1.column == 1 && token2.line == 2 &&
           token2.column == 3 && token3.line == 2 && token3.column == 8 &&
           token4.line == 2 && token4.column == 10 && token5.line == 3 &&
           token5.column == 1 && token1.is_valid() && token2.is_valid() &&
           token3.is_valid() && token4.is_valid() && token5.is_valid();
}

// Value extraction with std::optional
constexpr bool test_value_extraction()
{
    // Test boolean extraction (constexpr)
    Token true_token{TokenType::BOOLEAN_TRUE, "true"};
    Token false_token{TokenType::BOOLEAN_FALSE, "false"};

    auto true_val = extract_boolean(true_token);
    auto false_val = extract_boolean(false_token);

    // Test string extraction (constexpr)
    Token string_token{TokenType::STRING, "test"};
    auto str_val = extract_string(string_token);

    return true_val.has_value() && true_val.value() == true &&
           false_val.has_value() && false_val.value() == false &&
           str_val.has_value() && str_val.value() == "test";
}

// Complex JSON parsing test
constexpr bool test_complex_json()
{
    Tokenizer tokenizer(complex_test_json);

    // Test first few tokens to verify complex JSON works
    Token token1 = tokenizer.next_token(); // {
    Token token2 = tokenizer.next_token(); // "user"
    Token token3 = tokenizer.next_token(); // :
    Token token4 = tokenizer.next_token(); // {
    Token token5 = tokenizer.next_token(); // "id"
    Token token6 = tokenizer.next_token(); // :
    Token token7 = tokenizer.next_token(); // 12345

    return token1.type == TokenType::OBJECT_START &&
           token2.type == TokenType::STRING && token2.value == "user" &&
           token3.type == TokenType::COLON &&
           token4.type == TokenType::OBJECT_START &&
           token5.type == TokenType::STRING && token5.value == "id" &&
           token6.type == TokenType::COLON &&
           token7.type == TokenType::NUMBER && token7.value == "12345" &&
           token1.is_valid() && token2.is_valid() && token3.is_valid() &&
           token4.is_valid() && token5.is_valid() && token6.is_valid() &&
           token7.is_valid();
}

// Full tokenization test
constexpr bool test_full_tokenization()
{
    Tokenizer tokenizer(complex_test_json);

    size_t token_count = 0;
    Token token{TokenType::END_OF_INPUT};

    do {
        token = tokenizer.next_token();
        token_count++;

        // Safety check to prevent infinite loops in constexpr context
        if (token_count > 200) {
            return false; // Too many tokens, something's wrong
        }
    } while (token.type != TokenType::END_OF_INPUT && token.is_valid());

    // Expect approximately 100+ tokens for our complex JSON
    return token_count > 50 && token_count < 200 && token.is_valid();
}

// Comprehensive value extraction test
constexpr bool test_number_formats()
{
    // Test various number formats
    constexpr std::string_view negative_number = "-42.5";
    constexpr std::string_view integer = "12345";
    constexpr std::string_view scientific = "1.23e+10";
    constexpr std::string_view zero = "0";

    Tokenizer neg_tokenizer(negative_number);
    Tokenizer int_tokenizer(integer);
    Tokenizer sci_tokenizer(scientific);
    Tokenizer zero_tokenizer(zero);

    Token neg_token = neg_tokenizer.next_token();
    Token int_token = int_tokenizer.next_token();
    Token sci_token = sci_tokenizer.next_token();
    Token zero_token = zero_tokenizer.next_token();

    return neg_token.type == TokenType::NUMBER && neg_token.value == "-42.5" &&
           int_token.type == TokenType::NUMBER && int_token.value == "12345" &&
           sci_token.type == TokenType::NUMBER &&
           sci_token.value == "1.23e+10" &&
           zero_token.type == TokenType::NUMBER && zero_token.value == "0" &&
           neg_token.is_valid() && int_token.is_valid() &&
           sci_token.is_valid() && zero_token.is_valid();
}

// Error handling test
constexpr bool test_error_detection()
{
    // Test that basic error detection works in constexpr context
    constexpr std::string_view invalid_char = "@";

    Tokenizer tokenizer(invalid_char);
    Token token = tokenizer.next_token();

    return token.type == TokenType::ERROR &&
           token.error == ParseError::UNEXPECTED_CHARACTER &&
           token.is_error() && !token.is_valid();
}

// ============================================================================
// STATIC ASSERTIONS - All tests run at compile time
// ============================================================================

static_assert(test_tokenizer_construction(),
              "Tokenizer construction must be constexpr");
static_assert(test_string_tokenization(),
              "String tokenization must be constexpr");
static_assert(test_number_tokenization(),
              "Number tokenization must be constexpr");
static_assert(test_boolean_tokenization(),
              "Boolean tokenization must be constexpr");
static_assert(test_null_tokenization(), "Null tokenization must be constexpr");
static_assert(test_object_structure(),
              "Object structure parsing must be constexpr");
static_assert(test_array_structure(),
              "Array structure parsing must be constexpr");
static_assert(test_whitespace_handling(),
              "Whitespace handling must be constexpr");
static_assert(test_position_tracking(), "Position tracking must be constexpr");
static_assert(test_value_extraction(), "Value extraction must be constexpr");
static_assert(test_complex_json(), "Complex JSON parsing must be constexpr");
static_assert(test_full_tokenization(),
              "Complete JSON tokenization must be constexpr");
static_assert(test_number_formats(), "Number format parsing must be constexpr");
static_assert(test_error_detection(), "Error detection must be constexpr");

int main()
{
    // If we reach here, all static_assert tests passed at compile time!
    return 0;
}