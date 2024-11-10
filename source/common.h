//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef SIMPLELINEARREGRESSION_COMMON_H
#define SIMPLELINEARREGRESSION_COMMON_H

#include <stddef.h>
#include <stdlib.h>

// Standard DataPoint struct for x and y values. The attributes x and y are defined as pointers to
// allow for separate caching of the values.

typedef struct {
    double* x;
    size_t x_dimensions;
    double y;
} data_point_t;

// Data structure for pooling the read x values from the dataset. This is used to avoid multiple
// memory allocations for each x value by keeping the possibility to allow multidimensional x values
// at runtime without the need to change the underlying data structure or to rely on compile
// time macros.

typedef struct {
    double* x_value_pool;
    size_t x_value_pool_size;
} x_value_pool_t;

// Data structure for handling the dataset consisting of DataPoints + Owner of all allocated
// memory for the DataPoints.

typedef struct {
    data_point_t* data_points;
    size_t data_points_count;

} dataset_t;



void dataset_t_init(dataset_t* dataset, size_t data_points_count){
    dataset -> data_points = malloc(data_points_count * sizeof(data_point_t));
    dataset -> data_points_count = data_points_count;
}

void dataset_destroy(dataset_t* dataset) {
    free(dataset -> data_points);
}





#endif //SIMPLELINEARREGRESSION_COMMON_H
