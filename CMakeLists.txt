cmake_minimum_required(VERSION 3.27)
project(SimpleLinearRegression C)

set(CMAKE_C_STANDARD 11)

add_executable(SimpleLinearRegression
        example/main.c
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
