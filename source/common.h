//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef LINEARREGRESSION_C_COMMON_H
#define LINEARREGRESSION_C_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include "stdio.h"

#define GROWTH_FACTOR 2
#define GROWTH_THRESHOLD 1<<20
#define INITIAL_CAPACITY 10

// Standard DataPoint struct for x and y values. The attributes x and y are defined as pointers to
// allow for separate caching of the values.

typedef struct {
    double* x;
    double* y;
} data_point_t;

// Data structure for handling the dataset consisting of DataPoints + Owner of all allocated
// memory for the DataPoints.
// Complete x and y data is stored in the data_pool array. The DataPoints only store pointers to
// the data_pool array and work as a logical grouping. I chose this approach to allow for leveraging
// the cache more effectively and to reduce the number of memory allocations, especially if ever
// working with large datasets. This is hopefully a trade-off between complexity and performance.

typedef struct {
    data_point_t* data_points;
    size_t data_points_capacity;
    size_t data_points_count;
    size_t x_dimensions;
    double* data_pool;
} dataset_t;

void dataset_t_init_from_csv(dataset_t* dataset, const char* file_path);

void dataset_t_init(dataset_t* dataset, size_t x_dimensions);

void dataset_push_data_point(dataset_t* dataset, double* x, size_t x_dimensions, double y);

void dataset_destroy(dataset_t* dataset);






#endif //LINEARREGRESSION_C_COMMON_H
