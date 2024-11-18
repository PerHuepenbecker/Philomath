//
// Created by Per Hüpenbecker on 10.11.24.
//

#include "linreg.h"

void linear_regression_t_init(linear_regression_t* linreg, size_t dimensions, size_t epochs, double learning_rate){
    linreg->bias = 0.0;
    linreg->weights = calloc(dimensions, sizeof(double));
    linreg->learning_rate = learning_rate;
    linreg->mse = 0.0;
    linreg->standardize = false;
    linreg->x_means = NULL;
    linreg->x_stds = NULL;
    linreg->y_mean = 0.0;
    linreg->y_std = 0.0;
    linreg->dimensions = dimensions;
    linreg->epochs = epochs;
    linreg->data_points_count = 0;
}

void linear_regression_t_destroy(linear_regression_t* linreg){
    free(linreg->weights);
    if(linreg->x_means){
        free(linreg->x_means);
    }
    if (linreg->x_stds){
        free(linreg->x_stds);
    }
}