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