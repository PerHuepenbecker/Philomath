//
// Created by Per Hüpenbecker on 02.01.25.
//

#include "knn.h"



Result kNN_t_init(kNN_t* knn,dataset_t* dataset, size_t n_value, distance_type distance_value, inference_type inference_value){
    knn->n = n_value;
    knn->dataset = dataset;
    knn->distance_type = distance_value;
    knn->inference_type = inference_value;

    Result res;

    res = preprocessor_t_init(knn->preprocessor, dataset->dimensions);
    if (!res.is_ok){
        return Err_from(res.error);
    }
    res = preprocessor_t_fit(knn->preprocessor, knn->dataset);
    if (!res.is_ok){
        return Err_from(res.error);
    }
    res = preprocessor_t_transform(knn->preprocessor, knn->dataset);
    if (!res.is_ok){
        return Err_from(res.error);
    }

    return Ok(VOID);
}

static double kNN_t_euclidian_distance(kNN_t* knn, const double* data){

    double sum = 0.0;

    for(size_t i = 0; i < knn->dataset->dimensions-1; i++) {

        double difference = knn->dataset->data_points->line[i] - data[i];
        sum += difference * difference;

    }

    return sqrt(sum);
}

static void kNN_t_check_distance(kNN_t* knn, kNN_data* data_buffer, double distance, size_t index_dataset, double y_value_dataset){

    int candidate_index = -1;

    for (int i = 0; i < knn->n; i++) {

        if(data_buffer[i].distance == DBL_MAX){
            data_buffer[i] = (kNN_data){.dataset_idx = index_dataset, .distance = distance, .y_value = y_value_dataset};
            break;
        }

        if(distance < data_buffer[i].distance) {
            candidate_index = i;
        }
    }

    if(candidate_index >= 0) {
        data_buffer[candidate_index] = (kNN_data) {.dataset_idx = index_dataset, .distance = distance, .y_value = y_value_dataset};
    }
}

static void kNN_t_measure_k_distances(kNN_t* knn, kNN_data* data_buffer, double* data, size_t data_dimensions){

    double tmp_distance = 0.0;

    for (size_t i = 0; i < knn->dataset->data_points_count; i++) {
        tmp_distance = kNN_t_euclidian_distance(knn, data);
        kNN_t_check_distance(knn, data_buffer, tmp_distance, i, knn->dataset->data_points->line[data_dimensions]);
    }

}

static Result kNN_t_allocate_data_buffer(kNN_t* knn, kNN_data** data_buffer) {
    kNN_data* tmp = malloc(sizeof (kNN_data) * knn->n);

    if(tmp == NULL) {
        return Err(MEMORY_ALLOCATION_ERROR, "[kNN_t_predict] Error allocating kNN buffer\n", NULL);
    }

    *data_buffer = tmp;

    for (size_t i = 0; i < knn->n; i++) {
        (*data_buffer)[i] = (kNN_data) {.dataset_idx = 0, .distance = DBL_MAX, .y_value = 0.0};
    }

    return Ok(VOID);
}


double kNN_get_k_average_y_value(kNN_t* knn, kNN_data* data_buffer){

    double sum = 0.0;

    for(size_t i = 0; i < knn->n; i++) {
        sum += data_buffer[i].y_value;
    }

    return sum / (double)knn->n;
}

static Result kNN_regression(kNN_t* knn, kNN_data* data_buffer, double* result, double* data, size_t data_dimensions){

    kNN_t_measure_k_distances(knn, data_buffer, data,data_dimensions);
    double regression_result = kNN_get_k_average_y_value(knn, data_buffer);

    *result = regression_result;

    return Ok(VOID);
}


void kNN_sort_labels(kNN_t* knn, kNN_data* data_buffer){

    bool exchanged = true;
    size_t n = knn->n;

    while(exchanged) {
        exchanged = false;

        for (size_t i = 1; i < n; ++i) {
            if (data_buffer[i-1].y_value > data_buffer[i].y_value){
                kNN_data tmp = data_buffer[i];

                data_buffer[i] = data_buffer[i-1];

                data_buffer[i-1] = tmp;

                exchanged = true;
            }
        }
        n--;
    }
}

double kNN_find_most_frequent_label(kNN_t* knn, kNN_data* data_buffer) {

    // Memory allocation for worst case ==> n different labels

    label_count* label_counter = malloc(sizeof(label_count) * knn->n);
    if (!label_counter){
        fprintf(stderr, "[kNN_find_most_frequent_label] Memory allocation failed - Exiting.\n");
        exit(EXIT_FAILURE);
    }

    size_t unique_labels_count = 0;

    for (size_t i = 0; i < knn->n; i++) {
        bool found = false;
        for(size_t j = 0; j < unique_labels_count; j++){
            if(label_counter[j].label == data_buffer[i].y_value){
                label_counter[j].count++;
                found = true;
                break;
            }
        }

        if(!found) {
            label_counter[unique_labels_count].label = data_buffer[i].y_value;
            label_counter[unique_labels_count].count = 1;
            unique_labels_count++;
        }
    }

    double most_frequent = label_counter[0].label;
    size_t max_count = label_counter[0].count;

    for(size_t i = 1; i < unique_labels_count; i++){

        if(label_counter[i].count > max_count) {
            most_frequent = label_counter[i].label;
            max_count = label_counter[0].count;
        }
    }

    free(label_counter);

    return most_frequent;
}


static Result kNN_classification(kNN_t* knn, kNN_data* data_buffer, double* result, double* data, size_t data_dimensions){

    // Calculate the distance between each point and the data

    kNN_t_measure_k_distances(knn, data_buffer, data,data_dimensions);

    double classification_result = kNN_find_most_frequent_label(knn,data_buffer);
    *result = classification_result;

    return Ok(VOID);
}

Result kNN_t_predict(kNN_t* knn, double* result, double* data, size_t data_dimensions, size_t* index_y){

    if(data_dimensions != knn->dataset->dimensions-1) {
        return Err(DATA_CORRUPTION_ERROR, "[kNN_t_predict] Invalid data format. Data dimensions do not match with dataset\n", NULL);
    }

    if(knn->inference_type == REGRESSION && index_y == NULL || *index_y >= data_dimensions){
        return Err(INVALID_FUNCTION_ARGUMENT, "[kNN_t_predict] Invalid function argument. Index would be out of bonds of the dataset\n", NULL);
    }

    Result res;
    kNN_data* data_buffer = NULL;
    res = kNN_t_allocate_data_buffer(knn, &data_buffer);
    if(!res.is_ok) {
        return Err_from(res.error);
    }

    switch (knn->inference_type){
        case REGRESSION: kNN_regression(knn, data_buffer, result, data, data_dimensions);
            break;
        case CLASSIFICATION: kNN_classification(knn, data_buffer, result, data, data_dimensions);
            break;
        default:
            return Err(INVALID_STATE_ERROR, "[kNN_t_predict] Invalid kNN task. Inference type must be REGRESSION or CLASSIFICATION", NULL);
    }

    return Ok(VOID);
};
