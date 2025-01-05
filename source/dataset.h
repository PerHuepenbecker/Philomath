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
#define STANDARD_LABEL_BUFFER_SIZE 256

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

// Data type enum for possible handling of labeled and unlabeled data to be parsed form csv or other sources
// since labeled data requires different handling

typedef enum {
    LABELED_DATA,
    UNLABELED_DATA
} DATA_TYPE;

typedef enum {
    UNINITIALIZED,
    INITIALIZED,
    CORRUPTED
} DATASET_STATE;

// Struct for handling the data labels. Double numerical for either numerical labels or transformated texttual labels
// Optional char* for textual labels from csv

typedef struct {
    double numerical_label;
    char* text_label;
} data_label;

typedef void* CTX;

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

    DATASET_STATE state;
    DATA_TYPE data_type;

    size_t labels_capacity;
    size_t labels_count;
    char** labels;
} dataset_t;

Result dataset_t_init_from_csv(dataset_t*, const char*,  DATA_TYPE data_type);

Result dataset_t_init(dataset_t* dataset, size_t dimensions, DATA_TYPE);

Result data_handler(const char*,size_t, CTX);

Result data_handler_token_pointer(char** token_pointers, size_t token_count, CTX context);

void dataset_t_inspect(dataset_t* dataset, size_t num_data_points);

Result dataset_t_push_data_point(dataset_t* dataset, const double* data, size_t dimensions);
Result dataset_t_push_label(dataset_t* dataset, char* token_pointer, double* label_index);

Result dataset_t_map_index(dataset_t* dataset, char** label, double index);

void dataset_t_print_labels(dataset_t* dataset);

void dataset_t_destroy(dataset_t* dataset);


#endif //LINEARREGRESSION_C_COMMON_H
