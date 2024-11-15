//
// Created by Per Hüpenbecker on 15.11.24.
//

#ifndef SIMPLELINEARREGRESSION_RESULT_H
#define SIMPLELINEARREGRESSION_RESULT_H

// Definition of neccessary marcos for implementing a rust inspired Result type in C.
// The Result type is a generic type that can either be an Ok or an Err type.
#define Result(type, error_type)    \
                                    \
    struct {                        \
        int is_ok;                  \
        union {                     \
            type ok;                \
            struct {                \
                error_type err;     \
                const char* msg;    \
            } error_t;                \
                                    \
        };                          \
    }

#define Ok(result_type, value) ((result_type){ .is_ok = 1, .ok = (value) })
#define Err(result_type, error, message) ((result_type){ .is_ok = 0, .error_t = {.err= (error), .msg = (message)}})

#endif //SIMPLELINEARREGRESSION_RESULT_H
