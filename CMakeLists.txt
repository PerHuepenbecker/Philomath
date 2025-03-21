cmake_minimum_required(VERSION 3.27)

project(Philomath C)

include(ctest)

set(CMAKE_C_STANDARD 11)

add_library(Philomath_lib
        source/linreg.h
        source/dataset.c
        source/dataset.h
        source/linreg.c
        source/csv.h
        source/csv.c
        source/result.h
        source/preprocessor.c
        source/preprocessor.h
        source/result.c
        source/knn.c
        source/knn.h
)

add_executable(SimpleLinearRegression
        example/main.c)

add_executable(Test_CSV
        tests/test_csv.c
        tests/helpers.h
        tests/helpers.c)

add_executable(Test_Dataset
        tests/test_dataset.c
        tests/helpers.h
        tests/helpers.c)

add_executable(Test_Preprocessor
        tests/test_preprocessor.c
        tests/helpers.h
        tests/helpers.c)

target_link_libraries(SimpleLinearRegression PRIVATE Philomath_lib)
target_link_libraries(Test_CSV PRIVATE Philomath_lib)
target_link_libraries(Test_Dataset PRIVATE Philomath_lib)
target_link_libraries(Test_Preprocessor PRIVATE Philomath_lib)

enable_testing()

add_test(NAME test_csv COMMAND Test_CSV)
add_test(NAME test_dataset COMMAND Test_Dataset)
add_test(NAME test_preprocessor COMMAND Test_Preprocessor)