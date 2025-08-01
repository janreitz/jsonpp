# Simple tokenizer test example
add_executable(tokenizer_test tokenizer_test.cpp)
target_link_libraries(tokenizer_test PRIVATE jsonpp::jsonpp)