#include <fstream>
#include <iostream>
#include <jsonpp/tokenizer.hpp>
#include <sstream>

using namespace jsonpp;

const char *token_type_name(TokenType type)
{
    switch (type) {
    case TokenType::STRING:
        return "STRING";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::BOOLEAN_TRUE:
        return "TRUE";
    case TokenType::BOOLEAN_FALSE:
        return "FALSE";
    case TokenType::NULL_VALUE:
        return "NULL";
    case TokenType::OBJECT_START:
        return "OBJECT_START";
    case TokenType::OBJECT_END:
        return "OBJECT_END";
    case TokenType::ARRAY_START:
        return "ARRAY_START";
    case TokenType::ARRAY_END:
        return "ARRAY_END";
    case TokenType::COLON:
        return "COLON";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::END_OF_INPUT:
        return "END_OF_INPUT";
    }
    return "UNKNOWN";
}

int main()
{
    try {
        // Read test JSON file
        std::ifstream file("test_data.json");
        if (!file.is_open()) {
            std::cerr << "Could not open test_data.json\n";
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json_content = buffer.str();

        std::cout << "Tokenizing JSON file...\n";
        std::cout << "Input size: " << json_content.size() << " bytes\n\n";

        Tokenizer tokenizer(json_content);
        int token_count = 0;

        Token token{TokenType::END_OF_INPUT};
        do {
            token = tokenizer.next_token();
            token_count++;

            std::cout << "[" << token.line << ":" << token.column << "] "
                      << token_type_name(token.type);

            if (!token.value.empty()) {
                std::cout << " = \"" << token.value << "\"";
            }

            std::cout << "\n";

            // Limit output for very large files
            if (token_count > 100) {
                std::cout << "... (truncated after 100 tokens)\n";
                break;
            }

        } while (token.type != TokenType::END_OF_INPUT);

        std::cout << "\nTokenization complete. Total tokens: " << token_count
                  << "\n";

        // Test value extraction
        std::cout << "\nTesting value extraction:\n";

        // Test simple JSON snippets
        std::string test_cases[] = {R"("hello world")", "42.5", "true", "false",
                                    "null"};

        for (const auto &test : test_cases) {
            Tokenizer test_tokenizer(test);
            Token test_token = test_tokenizer.next_token();

            std::cout << "Input: " << test << " -> ";

            switch (test_token.type) {
            case TokenType::STRING:
                std::cout << "String: \"" << extract_string(test_token)
                          << "\"\n";
                break;
            case TokenType::NUMBER:
                std::cout << "Number: " << extract_number(test_token) << "\n";
                break;
            case TokenType::BOOLEAN_TRUE:
            case TokenType::BOOLEAN_FALSE:
                std::cout << "Boolean: "
                          << (extract_boolean(test_token) ? "true" : "false")
                          << "\n";
                break;
            case TokenType::NULL_VALUE:
                std::cout << "Null value\n";
                break;
            default:
                std::cout << "Other type\n";
                break;
            }
        }

    } catch (const TokenizerError &e) {
        std::cerr << "Tokenizer error at " << e.line() << ":" << e.column()
                  << ": " << e.what() << "\n";
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}