//
// Created by Per Hüpenbecker on 02.01.25.
//

#include "knn.h"



Result kNN_t_init(kNN_t* knn,dataset_t* dataset, size_t n_value, distance_type distance_value, inference_type inference_value){
    knn->n = n_value;
    knn->dataset = dataset;
    knn->distance_type = distance_value;
    knn->inference_type = inference_value;

    printf("[KNN] Base types initialized\n");

    Result res;

    res = preprocessor_t_init(&knn->preprocessor, dataset->dimensions, NO_TARGET_TRANSFORM);
    if (!res.is_ok){
        return Err_from(res.error);
    }

    printf("[KNN] Preprocessor initialized\n");

    res = preprocessor_t_fit(&knn->preprocessor, knn->dataset);
    if (!res.is_ok){
        return Err_from(res.error);
    }

    printf("[KNN] Preprocessor fitted\n");

    res = preprocessor_t_transform(&knn->preprocessor, knn->dataset);
    if (!res.is_ok){
        return Err_from(res.error);
    }

    printf("[KNN] Dataset transformed\n");

    return Ok(VOID);
}

static double kNN_t_euclidian_distance(kNN_t* knn, size_t index, double* data){

    double sum = 0.0;

    for(size_t i = 0; i < knn->dataset->dimensions-1; i++) {

        double difference = knn->dataset->data_points[index].line[i] - data[i];

        printf("[KNN DISTANCE] Calculating %f - %f \n", knn->dataset->data_points[index].line[i] , data[i]);

        printf("[KNN DISTANCE] Pow: %f x %f = %f\n", difference, difference, difference*difference);
        sum += (difference * difference);

    }
    printf("[KNN Distance] Sum: %f\n", sum);

    printf("[KNN Distance] Returning %f\n", sqrt(sum));
    return sqrt(sum);
}

static void kNN_t_check_distance(kNN_t* knn, kNN_data* data_buffer, double distance, size_t index_dataset, double y_value_dataset){

    int candidate_index = -1;

    for (int i = 0; i < knn->n; i++) {

        if(data_buffer[i].distance == DBL_MAX){
            data_buffer[i] = (kNN_data){.dataset_idx = index_dataset, .distance = distance, .y_value = y_value_dataset};

            printf("[KNN CHECK DISTANCE] Pushed: Index %zu, Distance %f, Y Value %f \n\n", data_buffer[i].dataset_idx,
                   data_buffer[i].distance, data_buffer[i].y_value);

            break;
        }

        if(distance < data_buffer[i].distance) {

            if(candidate_index < 0) {
                candidate_index = i;
            } else {
                if (data_buffer[i].distance > data_buffer[candidate_index].distance) {
                    candidate_index = i;
                }
            }

        }
    }

    if(candidate_index >= 0) {

        printf("[KNN CHECK DISTANCE] Replacing at Candidate %d Index %zu, Distance %f, Y Value %f\n", candidate_index, data_buffer[candidate_index].dataset_idx,
               data_buffer[candidate_index].distance, data_buffer[candidate_index].y_value);
        printf("\tWith: Index %zu, Distance %f, Y Value %f\n", index_dataset, distance, y_value_dataset);

        data_buffer[candidate_index] = (kNN_data) {.dataset_idx = index_dataset, .distance = distance, .y_value = y_value_dataset};
    }
}

static void kNN_t_measure_k_distances(kNN_t* knn, kNN_data* data_buffer, double* data, size_t data_dimensions){

    double tmp_distance = 0.0;

    for (size_t i = 0; i < knn->dataset->data_points_count; i++) {
        tmp_distance = kNN_t_euclidian_distance(knn, i, data);

        printf("[KNN Distance] Current distance: %f\n", tmp_distance);

        kNN_t_check_distance(knn, data_buffer, tmp_distance, i, knn->dataset->data_points[i].line[data_dimensions]);
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

    for(size_t i = 0; i < knn->n; i++) {
        printf("%zu, %f, %f\n", data_buffer[i].dataset_idx, data_buffer[i].distance, data_buffer[i].y_value);
    }

    *result = classification_result;

    return Ok(VOID);
}

Result kNN_t_predict(kNN_t* knn, double* result, double* data, size_t data_dimensions){

    if(data_dimensions != knn->dataset->dimensions-1) {
        return Err(DATA_CORRUPTION_ERROR, "[kNN_t_predict] Invalid data format. Data dimensions do not match with dataset\n", NULL);
    }

    Result res;
    kNN_data* data_buffer = NULL;
    res = kNN_t_allocate_data_buffer(knn, &data_buffer);
    if(!res.is_ok) {
        return Err_from(res.error);
    }

    for (size_t i = 0; i < data_dimensions; ++i) {
        double mean = knn->preprocessor.column_means[i];
        double std = knn->preprocessor.column_stds[i];

        printf("[KNN Predict] Mean: %f, Std: %f", mean, std);

        printf("[KNN Predict] Transforming input data: %f => %f\n", data[i], (data[i]-mean)/std);

        data[i] = (data[i]-mean)/std;

        printf("[KNN Predict] Input data at %d : %f\n", i, data[i]);
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
