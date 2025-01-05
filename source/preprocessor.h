//
// Created by Per Hüpenbecker on 18.11.24.
//

#ifndef SIMPLELINEARREGRESSION_PREPROCESSOR_H
#define SIMPLELINEARREGRESSION_PREPROCESSOR_H

#include "dataset.h"

typedef enum {
    TARGET_TRANSFORM,
    NO_TARGET_TRANSFORM
} target_transformation_option;

typedef enum {
    DATASET_FITTED,
    DATASET_STANDARDIZED,
    NO_STANDARDIZATION,
    UNKNOWN
} data_state_t;

typedef struct {
    size_t dimensions;
    double* column_means;
    double* column_stds;

    target_transformation_option target_transform;

    data_state_t state;
} preprocessor_t;

Result preprocessor_t_init(preprocessor_t*, size_t, target_transformation_option);
void preprocessor_t_destroy(preprocessor_t*);

Result preprocessor_t_fit(preprocessor_t*, dataset_t*);
Result preprocessor_t_transform(preprocessor_t*, dataset_t*);
//void preprocessor_t_fit_transform(preprocessor_t*, dataset_t*);

Result preprocessor_t_standardize_features(dataset_t*, size_t);
Result preprocessor_t_unnormalize(preprocessor_t*, dataset_t*);

Result preprocessor_t_get_column_stds(preprocessor_t*, double*);
Result preprocessor_t_get_column_means(preprocessor_t*, double*);


#endif //SIMPLELINEARREGRESSION_PREPROCESSOR_H
