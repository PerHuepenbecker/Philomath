//
// Created by Per Hüpenbecker on 18.11.24.
//

#include "preprocessor.h"
#include "math.h"

// Initializer for the preprocessor
void preprocessor_t_init(preprocessor_t* preprocessor, size_t dimensions){
    preprocessor->dimensions = dimensions;
    preprocessor->column_means = calloc(dimensions, sizeof(double));
    preprocessor->state = NO_STANDARDIZATION;
    if (preprocessor->column_means == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    preprocessor->column_stds = calloc(dimensions, sizeof(double));
    if (preprocessor->column_stds == NULL) {
        free(preprocessor->column_means);
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
}

// destructor for the preprocessor
void preprocessor_t_destroy(preprocessor_t* preprocessor){
    free(preprocessor->column_means);
    free(preprocessor->column_stds);
}

// Function to fit the preprocessor to the dataset. The function calculates the mean and standard deviation for each column

bool preprocessor_t_fit(preprocessor_t* preprocessor, dataset_t* dataset){

    if(preprocessor->dimensions != dataset->dimensions){
        fprintf(stderr, "Error: Dimensions of preprocessor do not match dataset dimensions\n");
        return false;
    }
    // calculate the mean for each column
    for(size_t i = 0; i < preprocessor->dimensions; i++){
        double sum = 0.0, sum_sq = 0.0, mean = 0.0;

        for(size_t j = 0;j < dataset->data_points_count; j++){
            double value =  dataset->data_points[j].line[i];
            sum += value;
            sum_sq += value * value;
        }

        mean = sum / dataset->data_points_count;
        preprocessor->column_means[i] = mean;
        preprocessor->column_stds[i] = sqrt(sum_sq/ dataset->data_points_count - mean * mean);
    }

    preprocessor->state = DATASET_FITTED;

    return true;
}

// Function to standardize the features of the dataset. The function takes a initialized preprocessor and a dataset as
// input and modifies the dataset in place. The function standardizes the features of the dataset by performing the
// z-score normalization.

void preprocessor_t_transform(preprocessor_t* preprocessor, dataset_t* dataset){
    if(preprocessor->dimensions != dataset->dimensions){
        fprintf(stderr, "Error: Dimensions of preprocessor do not match dataset dimensions\n");
        return;
    }

    if(preprocessor->state != DATASET_FITTED){
        fprintf(stderr, "Error: Preprocessor not fitted\n");
        return;
    }

    for(size_t i = 0; i<preprocessor->dimensions; i++){

            double mean = preprocessor->column_means[i];
            double std = preprocessor->column_stds[i];

            for (size_t j = 0; j < dataset->data_points_count; j++){
                dataset->data_points[j].line[i] = (dataset->data_points[j].line[i] - mean) / std;
            }
        }

    preprocessor->state = DATASET_STANDARDIZED;
}

// Helper function to fit and transform the dataset in one step

void preprocessor_t_fit_and_transform (preprocessor_t* preprocessor, dataset_t* dataset){
    preprocessor_t_fit(preprocessor, dataset);
    preprocessor_t_transform(preprocessor, dataset);
}

// Getter method for the column means - necessary for the linear regression model to make predictions based on the
// standardized data set.

void preprocessor_t_get_column_means(preprocessor_t* preprocessor, double* buffer){
    if (preprocessor->state == DATASET_FITTED){
        memcpy(buffer, preprocessor->column_means, preprocessor->dimensions * sizeof(double));
        return;
    }
    fprintf(stderr, "Error: Dataset currently not fitted!\n");
}

// Getter method for the column standard deviations - necessary for the linear regression model to make predictions based on the
// standardized data set.

void preprocessor_t_get_column_stds(preprocessor_t* preprocessor, double* buffer){
    if(preprocessor->state == DATASET_FITTED){
        memcpy(buffer, preprocessor->column_stds, preprocessor->dimensions * sizeof(double));
        return;
    }
    fprintf(stderr, "Error: Dataset currently not fitted!\n");
}

void preprocessor_t_unnormalize(preprocessor_t* preprocessor, dataset_t* dataset){
    if(preprocessor->state != DATASET_STANDARDIZED){
        fprintf(stderr, "Error: Dataset currently not standardized!\n");
        return;
    }

    for(size_t i = 0; i<preprocessor->dimensions; i++){

            double mean = preprocessor->column_means[i];
            double std = preprocessor->column_stds[i];

            for (size_t j = 0; j < dataset->data_points_count; ++j) {
                dataset->data_points[j].line[i] = dataset->data_points[j].line[i] * std + mean;
            }
        }

}

