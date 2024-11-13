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

static bool parse_csv_line(const char* line, enum ParseMode mode, size_t* x_dimensions, double* value_pool, ) {
    switch (mode) {
        case READ_X_DIMENSIONS: {
            char *token = strtok(line, ",");
            size_t count = 0;
            while (token != NULL) {
                count++;
                token = strtok(NULL, ",");
            }
            *x_dimensions = count;

        }
            break;
        case READ_DATA_POINTS:
            break;
        default:
            fprintf(stderr, "Error: Invalid parse mode\n");
            return false;
    }
};

static bool read_csv_file(dataset_t* dataset, const char* file_path){
    FILE* file = fopen(file_path, "r");

    if (!file){
        fprintf(stderr, "Error: Could not open file\n");
        return false;
    }

    char* line_buffer = malloc(sizeof(char) * STANDARD_BUFFER_SIZE);
    if (!line_buffer){
        fprintf(stderr, "Error: Memory allocation for buffer failed\n");
        return false;
    }
    // reading the first line to determine the number of dimensions as well as the names
    // of the following columns
    size_t line_length = 0;
    size_t buffer_size = STANDARD_BUFFER_SIZE;

    // Check input from first line of the csv file + necsessary cleanup on error

    if((line_length = getline(&line_buffer, &buffer_size, file)) == -1) {
        fprintf(stderr, "Error: Could not read first line - check file!\n");
        free(line_buffer);
        fclose(file);
        return false;
    }

    if (getline(&line_buffer, &buffer_size, file) == -1){
        fprintf(stderr, "Error: Could not read first line\n");
        return NULL;
    }

    return NULL;
}

// Function to initialize a dataset from a CSV file. The function reads the file line by line.
// The first line gets evaluated to determine the number of dimensions of the x values, so
// it relies on proper CSV formatting. The function then reads the remaining lines and stores
// the data in the dataset struct. The function returns true if the initialization was successful.

bool dataset_init_from_csv(dataset_t* dataset, const char* file_path){

    char* read_buffer = malloc(sizeof(char) * 1024);
    if (!read_buffer){
        fprintf(stderr, "File: Memory allocation for buffer failed\n");
        return false;
    }

    size_t x_dimensions = 0;
    size_t data_points_count = 0;



    free(read_buffer);

    return true;
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

        // Update of the stored pointers of the data_points array structs. Neccessary because
        // realloc might invalidate the existing pointers on reallocation.

        for(size_t i = 0; i < dataset->data_points_count; i++){
            dataset->data_points[i].x = &dataset->data_pool[i * (x_dimensions + 1)];
            dataset->data_points[i].y = &dataset->data_pool[i * (x_dimensions + 1) + x_dimensions];
        }

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

