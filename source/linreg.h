//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef LINEARREGRESSION_C_LINREG_H
#define LINEARREGRESSION_C_LINREG_H

#define INITIAL_MSE DBL_MAX;

#include <stddef.h>
#include <stdbool.h>
#include <float.h>
#include "preprocessor.h"
#include "dataset.h"

typedef enum {
    UNTRAINED,
    TRAINED
} regression_state;

typedef struct {
    double bias;
    double* weights;
    double learning_rate;
    double mse;

    regression_state regr_state;
    data_state_t data_state;

    double* column_means;
    double* column_stds;
    double y_mean;
    double y_std;

    size_t dimensions;
    size_t epochs;
    size_t data_points_count;
} linear_regression_t;

typedef struct{
    double bias;
    double* weights;
    size_t dimensions;
} linear_predictor_t;

double linear_predictor_predict(linear_predictor_t* predictor, double* x);

linear_predictor_t* linear_regression_export_predictor(linear_regression_t* regression);

void linear_regression_t_init(linear_regression_t* linreg, size_t dimensions, size_t epochs, double learning_rate);

void linear_regression_t_destroy(linear_regression_t* linreg);

void linear_regression_t_fit(linear_regression_t* linreg, dataset_t* dataset, size_t index_y);

void linear_regression_t_predict(linear_regression_t* linreg, double* x, double*y);

void linear_regression_t_inspect(linear_regression_t* linreg);

void linear_regression_t_sync_preprocessor(linear_regression_t* linreg, preprocessor_t* preprocessor);

void unnormalize_theta_values(linear_regression_t* linreg, size_t index_y);





#endif //LINEARREGRESSION_C_LINREG_H

