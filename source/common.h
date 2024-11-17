//
// Created by Per Hüpenbecker on 10.11.24.
//

#ifndef LINEARREGRESSION_C_COMMON_H
#define LINEARREGRESSION_C_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csv.h"

#define GROWTH_FACTOR 2
#define GROWTH_THRESHOLD 1<<20
#define INITIAL_CAPACITY 10
#define STANDARD_BUFFER_SIZE 1024

// Standard DataPoint struct for x and y values. The attributes x and y are defined as pointers to
// allow for separate caching of the values.

typedef struct {
    double* line;
} data_point_t;

// Enum for the different parsing modes of the CSV file.

enum ParseMode {
    READ_DATA_POINTS,
    READ_X_DIMENSIONS,
    READ_COLUMN_NAMES
};

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
    size_t dimensions;
    char** column_names;
    double* token_transformation_buffer;
    double* data_pool;
} dataset_t;

bool dataset_t_init_from_csv(dataset_t* dataset, const char* file_path);

void dataset_t_init(dataset_t* dataset, size_t x_dimensions);

bool data_handler(const char* tokenized_line,size_t token_count, void* context);

void dataset_t_inspect(dataset_t* dataset, size_t number_of_data_points);

void dataset_t_push_data_point(dataset_t* dataset, const double* data, size_t dimensions);

void dataset_t_destroy(dataset_t* dataset);

#endif //LINEARREGRESSION_C_COMMON_H
