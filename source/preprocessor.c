//
// Created by Per Hüpenbecker on 18.11.24.
//

#include "preprocessor.h"
#include "math.h"

// Initializer for the preprocessor
void preprocessor_t_init(preprocessor_t* preprocessor, size_t y_index, size_t dimensions, bool standardize_features, bool standardize_target){
    preprocessor->y_index = y_index;
    preprocessor->dimensions = dimensions;
    preprocessor->standardize_features = standardize_features;
    preprocessor->standardize_target = standardize_target;
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

    return true;
}

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
        if( (i != preprocessor->y_index && preprocessor->standardize_features) ||
            (i == preprocessor->y_index && preprocessor->standardize_target)){

            double mean = preprocessor->column_means[i];
            double std = preprocessor->column_stds[i];

            for (size_t j = 0; j < dataset->data_points_count; j++){
                dataset->data_points[j].line[i] = (dataset->data_points[j].line[i] - mean) / std;
            }
        }
    }
}

void preprocessor_t_fit_and_transform (preprocessor_t* preprocessor, dataset_t* dataset){
    preprocessor_t_fit(preprocessor, dataset);
    preprocessor_t_transform(preprocessor, dataset);
}


void preprocessor_t_unnormalize(preprocessor_t* preprocessor, double* value){
    if(preprocessor->state != DATASET_STANDARDIZED){
        fprintf(stderr, "Error: Dataset currently not standardized!\n");
        return;
    }

    for(size_t i = 0; i<preprocessor->dimensions; i++){
        if( (i != preprocessor->y_index && preprocessor->standardize_features) ||
            (i == preprocessor->y_index && preprocessor->standardize_target)){

            double mean = preprocessor->column_means[i];
            double std = preprocessor->column_stds[i];

            *value = (*value * std) + mean;
        }
    }

}

