//
// Created by Per Hüpenbecker on 15.11.24.
//

#ifndef SIMPLELINEARREGRESSION_RESULT_H
#define SIMPLELINEARREGRESSION_RESULT_H

#include <stdbool.h>


#define VOID NULL
#define MAX_LOG_SIZE 128

typedef enum{
    FILE_ERROR,
    FILE_NOT_FOUND_ERROR,
    FILE_PARSING_ERROR,
    STRUCT_INITIALIZATION_ERROR,
    INVALID_FUNCTION_ARGUMENT,
    MEMORY_ALLOCATION_ERROR,
    DATA_CONVERSION_ERROR,
    DATA_CORRUPTION_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR
} ErrorCode;

// I went here for a more object-oriented way and less usual C. Despite the overhead involved i wanted to ty to build
// a modular and generic error handling approach that enforces proper validation of function return values.

// Basic error struct. Allows handling of an error code and an error message

typedef struct {
    ErrorCode code;
    const char* message;
    const char* context;
} Error;



// Basic result type. The is_ok boolean determines if the Result is valid or not and can be used to shortcut
// result processing if using with usually void returning functions.

typedef struct {
    bool is_ok;
    union {
        void* data;
        Error error;
    };
} Result;

Result Err(ErrorCode code, const char* message, const char* context);

Result Err_from(Error error);

Result Ok(void* data);

Error get_error(Result result);

#endif //SIMPLELINEARREGRESSION_RESULT_H
