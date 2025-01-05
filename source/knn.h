//
// Created by Per Hüpenbecker on 02.01.25.
//

#ifndef PHILOMATH_KNN_H
#define PHILOMATH_KNN_H

#include "dataset.h"
#include "result.h"
#include "math.h"
#include "preprocessor.h"
#include "float.h"

// Enum to classify the type of distance used for k-NN tasks.

typedef enum {
    EUCLEDIAN,
} distance_type;

// Enum to classify the k-NN task. Regression will in base mode measure the average between the closest data points to
// determine a value y. Classification will work by making a majority decision based on the classes of the n closest points.

typedef enum {
    REGRESSION,
    CLASSIFICATION
} inference_type;

// Datastructure to hold the index in the dataset, the distance to the datapoint and the relevant value (label / regression y)

typedef struct {
    size_t dataset_idx;
    double distance;
    double y_value;
} kNN_data;

// Helper struct for counting the most frequent labels

typedef struct {
    double label;
    size_t count;
} label_count;

// Wrapper struct for the variables needed for the k-NN tasks regression and classification

typedef struct {

    size_t n;
    distance_type distance_type;
    inference_type inference_type;
    dataset_t* dataset;
    preprocessor_t* preprocessor;

} kNN_t;

Result kNN_t_init(kNN_t* knn,dataset_t* dataset, size_t n_value, distance_type distance_value, inference_type inference_value);
void kNN_t_destroy(kNN_t* knn);

// kNN predictor function for regression or classification. The result will be written to the result pointer.

Result kNN_t_predict(kNN_t* knn, double* result, double* data, size_t data_dimensions, size_t* index_y);


#endif //PHILOMATH_KNN_H
