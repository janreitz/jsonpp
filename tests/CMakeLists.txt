# Tokenizer test suite - all tests run at compile time via static_assert
add_executable(test_tokenizer test_tokenizer.cpp)
target_link_libraries(test_tokenizer PRIVATE jsonpp::jsonpp)

# If this compiles without errors, all tests have passed!