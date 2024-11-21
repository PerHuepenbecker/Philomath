//
// Created by Per Hüpenbecker on 15.11.24.
//

#ifndef SIMPLELINEARREGRESSION_RESULT_H
#define SIMPLELINEARREGRESSION_RESULT_H

/* Former Result handling approach via macro in the of Rust. I decided to go with another approach because
 * of the lacking flexibility and type safety by using this macro */

// Definition of neccessary marcos for implementing a rust inspired Result type in C.
// The Result type is a generic type that can either be an Ok or an Err type.
//#define Result(type, error_type)    \
                                    \
    struct {                        \
        int is_ok;                  \
        union {                     \
            type ok;                \
            struct {                \
                error_type err;     \
                const char* msg;    \
            } error_t;              \
                                    \
        };                          \
    }

//#define Ok(result_type, value) ((result_type){ .is_ok = 1, .ok = (value) })
//#define Err(result_type, error, message) ((result_type){ .is_ok = 0, .error_t = {.err= (error), .msg = (message)}})

// Enum of generic error codes - specific error information has to be handled in the message or in a polymorphic
// extension of the base error struct

typedef enum{
    FILE_ERROR,
    FILE_NOT_FOUND_ERROR,
    FILE_PARSING_ERROR,
    STRUCT_INITIALIZATION_ERROR,
    MEMORY_ALLOCATION_ERROR,
    INDEX_OUT_OF_BOUNDS_ERROR
} ErrorCode;

// I went here for a more object-oriented way and less usual C. Despite the overhead involved i wanted to ty to build
// a modular and generic error handling approach that enforces proper validation of function return values.

// Basic result type. The is_ok boolean determines if the Result is valid or not and can be used to shortcut
// result processing if using with usually void returning functions.

typedef struct {
    bool is_ok;
    void* data;
} Result;

// Basic error struct. Allows handling of an error code and an error message

typedef struct {
    ErrorCode code;
    char* message;
} Error;

Result Ok(void* data){
    Result res = {true, data};
    return res;
}

Result Err(Error *error) {
    Result res = {false, error};
    return res;
}

bool is_ok(Result result){
    return result.is_ok;
}

Error* get_error(Result result){
    Error* err = (result.is_ok) ? NULL : result.data;
    return err;
}

#endif //SIMPLELINEARREGRESSION_RESULT_H
