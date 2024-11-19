//
// Created by Per Hüpenbecker on 10.11.24.
//

#include "linreg.h"

void linear_regression_t_init(linear_regression_t* linreg, size_t dimensions, size_t epochs, double learning_rate){
    linreg->bias = 0.0;
    linreg->weights = calloc(dimensions, sizeof(double));
    linreg->learning_rate = learning_rate;
    linreg->mse = 0.0;
    linreg->column_means = calloc(dimensions, sizeof(double));
    linreg->column_stds = calloc(dimensions, sizeof(double));
    linreg->data_state = UNKNOWN;
    linreg->y_mean = 0.0;
    linreg->y_std = 0.0;
    linreg->dimensions = dimensions;
    linreg->epochs = epochs;
    linreg->data_points_count = 0;
}

void linear_regression_t_sync_preprocessor(linear_regression_t* linreg, preprocessor_t* preprocessor){
    if (linreg->dimensions != preprocessor->dimensions){
        fprintf(stderr, "Dimensions of linear regression model and preprocessed data do not match\n");
        return;
    }

    linreg->data_state = preprocessor->state;

    for(size_t i = 0; i < linreg->dimensions; i++){
        linreg->column_means[i] = preprocessor->column_means[i];
        linreg->column_stds[i] = preprocessor->column_stds[i];
    }
}

void linear_regression_t_destroy(linear_regression_t* linreg){
    free(linreg->weights);
    if(linreg->column_means){
        free(linreg->column_means);
    }
    if (linreg->column_stds){
        free(linreg->column_stds);
    }
}



void linear_regression_t_fit(linear_regression_t* linreg, dataset_t* dataset, size_t index_y){

    // Check if a valid y_index is being supplied to the linear regression mode

    if(index_y >= dataset->dimensions){
        fprintf(stderr, "Accessed index_y out of bounds\n");
        exit(1);
    }

    // Determination of x indices in the supplied dataset based of the supplied y_index. Used for easy
    // and readable access

    size_t* x_indices = (size_t*) malloc(sizeof(size_t) * linreg->dimensions-1);
    if(!x_indices){
        fprintf(stderr, "x_indices allocation failed\n");
        exit(1);
    }

    size_t x_index = 0;
    for (int i = 0; i < linreg->dimensions-1; ++i) {
        if (i != index_y){
            x_indices[x_index] = i;
            x_index++;
        }
    }

    // initialization of the necessary error tracking variables
    double sum_errors = 0.0;
    double* sum_weighted_errors = calloc(linreg->dimensions-1, sizeof(double));

    // initialization of the model bias and weights. Bias initialization is done by taking the mean
    // of the y values as recommended by some sources, weight initialization is currently done by constant
    // assignment of 0.1
    linreg->bias = linreg->column_means[index_y];
    for (int i = 0; i < linreg->dimensions; ++i) {
        linreg->weights[i] = 0.1;
    }

    // Actual training loop for a maximum of the specified epochs. Training will end early if the delta of the last mse
    // to the current mse is less than 0.000001 which means enough precision of this model. Alternative modifications
    // will definitely be added in the future

    size_t current_epoch = 0;
    double last_mse = 100.0;
    while(current_epoch < linreg->epochs) {

        double current_mse = 0.0;

        for (size_t i = 0; i < dataset->data_points_count; ++i) {

            // Handling of possibly multidimensional data by access though line pointers in the data points of the
            // dataset struct
            double portion_weighted_x = 0.0;

            for (size_t j = 0; j < linreg->dimensions-1; ++j) {
                double current_x_value = dataset->data_points[i].line[x_indices[j]];
                portion_weighted_x += linreg->weights[j]*current_x_value;
            }

            double prediction = linreg->bias += portion_weighted_x;
            double error = dataset->data_points->line[index_y] - prediction;

            current_mse += (error*error);

            sum_errors += error;

            for (size_t j = 0; j < linreg->dimensions-1; ++j) {
                sum_weighted_errors[j] = error * dataset->data_points[i].line[x_indices[j]];
            }
        }

        current_mse /= dataset->data_points_count;

        if (last_mse-current_mse < 0.000001) {
            break;
        }

        double mse_bias = sum_errors *= (-2.0/dataset->data_points_count);
        linreg->bias -= linreg->learning_rate*mse_bias;

        for (int i = 0; i < linreg->dimensions-1; ++i) {
            double mse_weight = sum_weighted_errors[i] *= (-2.0/dataset->data_points_count);
            linreg -> weights[i] -= linreg->learning_rate*mse_weight;
        }

        current_epoch++;
    }
}