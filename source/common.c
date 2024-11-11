//
// Created by Per Hüpenbecker on 10.11.24.
//

#include "common.h"

void dataset_t_init(dataset_t* dataset, size_t x_dimensions){
    dataset -> data_points = malloc(sizeof(data_point_t) * INITIAL_CAPACITY);
    dataset -> data_points_capacity = INITIAL_CAPACITY;
    dataset -> data_points_count = 0;
    dataset -> x_dimensions = x_dimensions;
    dataset -> data_pool = malloc(sizeof(double) * INITIAL_CAPACITY * (x_dimensions + 1));

    if(dataset -> data_points == NULL || dataset -> data_pool == NULL){
        fprintf(stderr, "Error: Initial memory allocation failed\n");
        exit(1);
    }
}

// Simple destructor. Frees all allocated memory. Here comes the approach of using centralized memory
// allocation into play. This way we can safely free all memory with a single call to free for each
// dynamic array.

void dataset_destroy(dataset_t* dataset) {
    free(dataset -> data_points);
    free(dataset -> data_pool);
}


void dataset_push_data_point(dataset_t* dataset, double* x, size_t x_dimensions, double y) {
    if (x_dimensions != dataset -> x_dimensions) {
        fprintf(stderr, "Error: Dimensions of x do not match dataset dimensions\n");
        return;
    }

    if(dataset -> data_points_capacity - dataset -> data_points_count == 0){
        void* tmp = realloc(dataset -> data_points, sizeof(data_point_t) * GROWTH_FACTOR);
        if(tmp == NULL){
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }

        for (size_t i = dataset->data_points_count; i < dataset->data_points_capacity; i++){
            dataset->data_points[i].x = NULL;
            dataset->data_points[i].y = NULL;
        }

        dataset -> data_points = tmp;

        tmp = realloc(dataset -> data_pool, sizeof(double) * GROWTH_FACTOR * (x_dimensions + 1));
        if(tmp == NULL){
            fprintf(stderr, "Error: Memory allocation failed\n");
            return;
        }

        dataset -> data_pool = tmp;
        dataset -> data_points_capacity *= GROWTH_FACTOR;
    }

    for(int i = 0; i < x_dimensions; i++){
        dataset -> data_pool[dataset -> data_points_count * (x_dimensions + 1) + i] = x[i];
        dataset -> data_pool[dataset -> data_points_count * (x_dimensions + 1) + x_dimensions] = y;
    }

    dataset -> data_points[dataset -> data_points_count].x = &dataset -> data_pool[dataset -> data_points_count * (x_dimensions + 1)];
    dataset -> data_points[dataset -> data_points_count].y = &dataset -> data_pool[dataset -> data_points_count * (x_dimensions + 1) + x_dimensions];
    dataset -> data_points_count++;

}