//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef LINEARREGRESSION_C_LINREG_H
#define LINEARREGRESSION_C_LINREG_H

#include <stddef.h>
#include <stdbool.h>
#include "preprocessor.h"
#include "common.h"

typedef struct {
    double bias;
    double* weights;
    double learning_rate;
    double mse;
    bool standardize;

    double* x_means;
    double* x_stds;
    double y_mean;
    double y_std;

    size_t dimensions;
    size_t epochs;
    size_t data_points_count;
} linear_regression_t;

void linear_regression_t_init(linear_regression_t* linreg, size_t dimensions, size_t epochs, double learning_rate);
void linear_regression_t_destroy(linear_regression_t* linreg);

void linear_regression_t_fit(linear_regression_t* linreg, dataset_t* dataset);
void linear_regression_t_predict(linear_regression_t* linreg, dataset_t* dataset, double* x, double* y);

void linear_regression_t_inspect(linear_regression_t* linreg);





#endif //LINEARREGRESSION_C_LINREG_H

