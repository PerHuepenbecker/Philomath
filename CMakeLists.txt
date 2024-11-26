cmake_minimum_required(VERSION 3.27)

project(Philomath C)

include(ctest)

set(CMAKE_C_STANDARD 11)

add_library(SimpleML
        source/linreg.h
        source/dataset.c
        source/dataset.h
        source/linreg.c
        source/csv.h
        source/csv.c
        source/result.h
        source/preprocessor.c
        source/preprocessor.h
        source/result.c)

add_executable(SimpleLinearRegression
        example/main.c)

add_executable(Tests
        tests/test_csv.c)

target_link_libraries(SimpleLinearRegression PRIVATE SimpleML)
target_link_libraries(Tests PRIVATE SimpleML)

enable_testing()

add_test(NAME test_csv COMMAND Tests)