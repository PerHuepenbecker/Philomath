//
// Created by Per Hüpenbecker on 18.11.24.
//

#ifndef SIMPLELINEARREGRESSION_PREPROCESSOR_H
#define SIMPLELINEARREGRESSION_PREPROCESSOR_H

#include "common.h"

typedef enum {
    DATASET_FITTED,
    FEATURES_STANDARDIZED,
    DATASET_STANDARDIZED,
    NO_STANDARDIZATION
} preprocessor_state_t;

typedef struct {
    size_t y_index;
    size_t dimensions;
    double* column_means;
    double* column_stds;
    bool standardize_features;
    bool standardize_target;
    preprocessor_state_t state;
} preprocessor_t;

void preprocessor_t_init(preprocessor_t*, size_t, size_t, bool, bool);
void preprocessor_t_destroy(preprocessor_t*);

bool preprocessor_t_fit(preprocessor_t*, dataset_t*);
void preprocessor_t_transform(preprocessor_t*, dataset_t*);
void preprocessor_fit_transform(preprocessor_t*, dataset_t*);

void preprocessor_t_standardize_features(dataset_t*, size_t);
void preprocessor_t_unnormalize(preprocessor_t*, dataset_t*);

void preprocessor_t_get_column_stds(preprocessor_t*, double*);
void preprocessor_t_get_column_means(preprocessor_t*, double*);


#endif //SIMPLELINEARREGRESSION_PREPROCESSOR_H
