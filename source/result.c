//
// Created by Per Hüpenbecker on 22.11.24.
//

#include "result.h"

Result Ok(void* data){
    return (Result) {.is_ok = true, .data=data};
}

Result Err(ErrorCode code, const char* message, const char* context) {
    return (Result){
            .is_ok = false,
            .error = {.code = code, .message = message, .context = context}
    };
}

Error get_error(Result result){
    return result.error;
}

