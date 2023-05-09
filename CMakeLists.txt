cmake_minimum_required(VERSION 3.16.0)

project(decoder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
  ${PROJECT_NAME}
  "src/main.cpp"
  "src/instruction_specification_builder.cpp"
  "src/instruction_printer.cpp"
  "src/instruction_decoder.cpp"
  "src/io_utils.cpp"
)

target_compile_options(${PROJECT_NAME} PRIVATE -W4 -WX)



add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/instructions_specification
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/instructions_specification)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/tests)


########################################################
# GOOGLE TEST
########################################################

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  decoder_tests
  "src/instruction_specification_builder.cpp"
  "src/instruction_printer.cpp"
  "src/instruction_decoder.cpp"
  "src/io_utils.cpp"

  "src/instruction_specification_builder_test.cpp"
)

target_link_libraries(
  decoder_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(decoder_tests)
