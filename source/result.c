//
// Created by Per Hüpenbecker on 22.11.24.
//

#include "result.h"

// Helper function to return a valid result

Result Ok(void* data){
    return (Result) {.is_ok = true, .data=data};
}

// Helper function to return an error result

Result Err(ErrorCode code, const char* message, const char* context) {
    return (Result){
            .is_ok = false,
            .error = {.code = code, .message = message, .context = context}
    };
}

// Helper function to perform error propagation

Result Err_from(Error err){
    return (Result){
        .is_ok = false,
        .error = {.code = err.code, .message = err.message, .context = err.context}
    };
}

// Helper function to extract an error out of a result

Error get_error(Result result){
    return result.error;
}

