//
// Created by Per Hüpenbecker on 10.11.24.
//

#include "dataset.h"

// Function to initialize the dataset with a specified number of dimensions. Used if the datasets internal buffers
// Part of the standard dataset initialization function and the push_data_point function if the dataset supplied her via
// the function argument is not initialized by now. The function initializes the data_points array with a initial capacity
// and sets the dimensions attribute of the dataset struct.
// Waiting for refactoring to a more generic approach to avoid code duplication but for now it works just fine.

Result dataset_t_init(dataset_t* dataset, size_t dimensions){
    dataset -> data_points = malloc(sizeof(data_point_t) * INITIAL_CAPACITY);

    if(!dataset->data_points){
        return Err(MEMORY_ALLOCATION_ERROR, "Allocation error of data points array", NULL);
    }

    for (int i = 0; i < INITIAL_CAPACITY; ++i) {
        dataset->data_points[i].line = NULL;
    }
    dataset -> data_points_capacity = INITIAL_CAPACITY;
    dataset -> data_points_count = 0;
    dataset -> dimensions = dimensions;
    dataset -> data_pool = malloc(sizeof(double) * INITIAL_CAPACITY * (dimensions));
    dataset -> token_transformation_buffer = NULL;
    if(dataset -> data_pool == NULL){
        free(dataset->data_points);
        return Err(MEMORY_ALLOCATION_ERROR, "Allocation error of data_pool array", NULL);
    }
}

static Result dataset_t_init_memory(dataset_t* dataset, size_t dimensions){
    dataset -> data_points = malloc(sizeof(data_point_t) * INITIAL_CAPACITY);

    if (!dataset->data_points){
        return Err(MEMORY_ALLOCATION_ERROR, "Allocation error of data points array", NULL);
    }
    for (int i = 0; i < INITIAL_CAPACITY; ++i) {
        dataset->data_points[i].line = NULL;
    }
    dataset -> data_points_capacity = INITIAL_CAPACITY;
    dataset -> data_points_count = 0;
    dataset -> dimensions = dimensions;
    dataset -> data_pool = malloc(sizeof(double) * INITIAL_CAPACITY * (dimensions));
    if (!dataset->data_pool){
        free(dataset->data_points);
        return Err(MEMORY_ALLOCATION_ERROR, "Allocation error of data_pool array", NULL);
    }
}


// Function to inspect the dataset. Prints the specified number of data points to the stdout.

void dataset_t_inspect(dataset_t* dataset, size_t num_data_points){

    printf("Dataset inspection:\n\n");

    for (size_t i = 0; i< num_data_points; i++){
        printf("Index: %zu: ", i);
        if (dataset->data_points[i].line == NULL) {
            fprintf(stderr, "Error: Data point not initialized\n");
            return;
        }
        for (size_t j = 0; j < dataset->dimensions; j++) {
            printf("%f ", dataset->data_points[i].line[j]);
        }
        printf("\n");
    }
}

// Simple destructor. Frees all allocated memory. Here comes the approach of using centralized memory
// allocation into play. This way we can safely free all memory with a single call to free for each
// dynamic array.

void dataset_t_destroy(dataset_t* dataset) {
    free(dataset -> data_points);
    free(dataset -> data_pool);
    if(dataset->token_transformation_buffer)
        free(dataset -> token_transformation_buffer);
}

Result data_handler_token_pointer(char ** token_pointer, size_t token_count, void* context){
    dataset_t* dataset = (dataset_t*) context;
    size_t processed_token = 0;

    if(dataset->token_transformation_buffer == NULL){
        dataset->token_transformation_buffer = malloc(sizeof(double) * token_count);
        if(dataset->token_transformation_buffer == NULL){
            return Err(MEMORY_ALLOCATION_ERROR, "Allocation of token transformation buffer failed.", NULL);
        }
    }

    dataset->dimensions = token_count;
    char* token_end;

    for (int i = 0; i < token_count; i++) {
        double token_value = strtod(token_pointer[i], &token_end);
        if (token_end == token_pointer[i]){
            char token_buf[MAX_LOG_SIZE];
            snprintf(token_buf, MAX_LOG_SIZE, "Token: %s", token_pointer[i]);
            return Err(DATA_CONVERSION_ERROR, "Token could not be processed", token_buf);
        }
        dataset->token_transformation_buffer[i] = token_value;
        processed_token++;
    }

    if (processed_token!=token_count){
        return Err(DATA_CORRUPTION_ERROR, "Error in data handler callback: Token count does not match x dimensions", NULL);
    }

    dataset_t_push_data_point(dataset, dataset->token_transformation_buffer, token_count);

    Ok(VOID);
}

Result data_handler(const char* tokenized_line,size_t token_count, void* context) {
    dataset_t *dataset = (dataset_t *) context;

    // Since the data_handler is the only function to use the token transformation buffer it will be lazily
    // allocated here. The buffer is used to store the transformed tokens for a quick validation of the line content
    // before pushing the data to the dataset. The buffer is freed when the dataset is destroyed but serves as a intermediate
    // storage for the data_handler callback and is reused for each line. This way we avoid frequent memory allocations
    // and deallocations but still have a clean interface for the data_handler callback.

    if (dataset->token_transformation_buffer == NULL) {
        dataset->token_transformation_buffer = malloc(sizeof(double) * token_count);
        //printf("Allocating token transformation buffer\n");
        if (dataset->token_transformation_buffer == NULL) {
            //fprintf(stderr, "Error: Memory allocation failed\n");
            return Err(MEMORY_ALLOCATION_ERROR, "Allocation of token transformation buffer failed.", NULL);
        }
    }

    dataset->dimensions = token_count;

    size_t processed_token = 0;
    char *token = (char *) tokenized_line;
    char *token_end;

    for (int i = 0; i < token_count; i++) {
        double token_value = strtod(token, &token_end);
        if (token_end == token) {
            char token_buf[MAX_LOG_SIZE];
            snprintf(token_buf, MAX_LOG_SIZE, "Token: %s", token);
            return Err(DATA_CONVERSION_ERROR, "Token could not be processed", tokenized_line);
        }
        dataset->token_transformation_buffer[i] = token_value;

        //printf("%d = %f\n", i,token_value);

        processed_token++;

        token = token_end+1;
    }

    if (processed_token != token_count) {
        return Err(DATA_CORRUPTION_ERROR, "Error in data handler callback: Token count does not match x dimensions", tokenized_line);
    }

    dataset_t_push_data_point(dataset, dataset->token_transformation_buffer, token_count);

    return Ok(VOID);
}

// Function to initialize a dataset from a CSV file. The function reads the file line by line.
// The first line gets evaluated to determine the number of dimensions of the x values, so
// it relies on proper CSV formatting. The function then reads the remaining lines and stores
// the data in the dataset struct. The function returns true if the initialization was successful.

Result dataset_t_init_from_csv(dataset_t* dataset, const char* file_path){

    Result temp_res;
    csv_parser_t parser;
    csv_parser_t_init_std(&parser);
    csv_callback_t callback = {
            .csv_callback_data = data_handler_token_pointer,
            .context = dataset
    };

    temp_res = csv_parser_parse(file_path, &parser, callback);
    if(!temp_res.is_ok){
        return Err_from(temp_res.error);
    }

    if (parser.has_header){
        dataset->column_names = malloc(sizeof(char*) * parser.column_count);
        if (!dataset->column_names){
            return Err(MEMORY_ALLOCATION_ERROR, "Memory allocation for columm headers of dataset struct failed.", file_path);
        }
        for (size_t i = 0; i < parser.column_count; ++i) {
            dataset->column_names[i] = malloc(sizeof(char) * strlen(parser.column_names[i]));
            if (!dataset->column_names[i]){
                return Err(MEMORY_ALLOCATION_ERROR, "Memory allocation for column name in dataset struct failed.", file_path);
            }
            strcpy(dataset->column_names[i], parser.column_names[i]);
        }
    }

    csv_parser_t_destroy(&parser);

    return Ok(VOID);
}

// Function to push a data point to the dataset. The function does the dimension check to allow for manual pushing of data
// points. The function reallocates memory if the data_points array is full. The data is stored in the data_pool array and
// referenced via line pointers by the data_points array.

Result dataset_t_push_data_point(dataset_t* dataset, const double* data, size_t dimensions) {
    if (dimensions != dataset -> dimensions) {
        return Err(DATA_CORRUPTION_ERROR, "Dimensions of dataset and data point do not match", NULL);
    }

    if(dataset->data_points_capacity == 0){
        dataset_t_init_memory(dataset, dimensions);
    }

    if(dataset -> data_points_capacity - dataset -> data_points_count == 0){

        printf("Reallocating memory\n");

        double* tmp_pool = realloc(dataset -> data_pool, sizeof(double)  * (dimensions) * dataset -> data_points_capacity * GROWTH_FACTOR);
        if(tmp_pool == NULL){
            return Err(MEMORY_ALLOCATION_ERROR, "Memory Allocation of data_pool failed", NULL);
        }

        dataset -> data_pool = tmp_pool;

        void* tmp = realloc(dataset -> data_points, dataset->data_points_capacity * GROWTH_FACTOR);
        if(tmp == NULL){
            return Err(MEMORY_ALLOCATION_ERROR, "Reallocation of data_points failed", NULL);
        }

        dataset -> data_points = tmp;

        // Update of the stored pointers of the data_points array structs. Neccessary because
        // realloc might invalidate the existing pointers on reallocation if the data pool gets
        // displaced.

        for(size_t i = 0; i < dataset->data_points_count; i++){
            dataset->data_points[i].line = &dataset->data_pool[i * (dimensions)];
        }

        for (size_t i = dataset->data_points_count; i < dataset->data_points_capacity; i++){
            dataset->data_points[i].line = NULL;
        }

        dataset -> data_points_capacity *= GROWTH_FACTOR;
    }

    for(int i = 0; i < dimensions; i++){
        dataset -> data_pool[dataset -> data_points_count * (dimensions) + i] = data[i];
    }

    dataset -> data_points[dataset -> data_points_count].line = &dataset -> data_pool[dataset -> data_points_count * (dimensions)];
    dataset -> data_points_count++;

    return Ok(VOID);
}

