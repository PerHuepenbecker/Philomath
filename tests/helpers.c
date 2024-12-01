//
// Created by Per Hüpenbecker on 01.12.24.
//

#include "helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

data_point_test_t* generate_test_dataset(TestDatasetType type){

    data_point_test_t* test_array = malloc(sizeof(data_point_test_t) * 12);

    switch (type) {
        case TEST_REGULAR:
            test_array[0] = (data_point_test_t){123.4, 234.5};
            test_array[1] = (data_point_test_t){345.6, 456.7};
            test_array[2] = (data_point_test_t){567.8, 678.9};
            test_array[3] = (data_point_test_t){789.0, 890.1};
            test_array[4] = (data_point_test_t){1011.2, 1112.3};
            test_array[5] = (data_point_test_t){1213.4, 1314.5};
            test_array[6] = (data_point_test_t){1415.6, 1516.7};
            test_array[7] = (data_point_test_t){1617.8, 1718.9};
            test_array[8] = (data_point_test_t){1819.0, 1920.1};
            test_array[9] = (data_point_test_t){2021.2, 2122.3};
            test_array[10] = (data_point_test_t){2223.4, 2324.5};
            test_array[11] = (data_point_test_t){2425.6, 2526.7};
            break;
        case TEST_LINREG_2DIM:
            test_array[0] = (data_point_test_t){1.0, 2.5};
            test_array[1] = (data_point_test_t){2.0, 3.5};
            test_array[2] = (data_point_test_t){3.0, 4.5};
            test_array[3] = (data_point_test_t){4.0, 5.5};
            test_array[4] = (data_point_test_t){5.0, 6.5};
            test_array[5] = (data_point_test_t){6.0, 7.5};
            test_array[6] = (data_point_test_t){7.0, 8.5};
            test_array[7] = (data_point_test_t){8.0, 9.5};
            test_array[8] = (data_point_test_t){9.0, 10.5};
            test_array[9] = (data_point_test_t){10.0, 11.5};
            test_array[10] = (data_point_test_t){11.0, 12.5};
            test_array[11] = (data_point_test_t){12.0, 13.5};
            break;
    }
    return test_array;
}

void write_example_files(){
    const char* regular_csv = "test_x,test_y\n"
                              "123.4,234.5\n"
                              "345.6,456.7\n"
                              "567.8,678.9\n";

    // Missing values in the csv file

    const char* missing_values_csv = "test_x,test_y\n"
                                     "123.4,234.5\n"
                                     "234.5";

    // Quoted values in the csv file

    const char* quoted_line_csv = "test_x, test_y\n"
                                  "\"\"Hello, World!\"\", Hello World!";

    // Header only csv file

    const char* header_csv = "test_x,test_y\n";

    FILE* file = fopen("test_1.csv", "w");
    fprintf(file, "%s", regular_csv);
    fclose(file);

    file = fopen("test_2.csv", "w");
    fprintf(file, "%s", missing_values_csv);
    fclose(file);

    file = fopen("test_3.csv", "w");
    fprintf(file, "%s", quoted_line_csv);
    fclose(file);

    file = fopen("test_4.csv", "w");
    fprintf(file, "%s", header_csv);
    fclose(file);
}

// Helper functions for the test context

void test_context_clear(test_context* ctx) {
    memset(ctx->buffer, 0, TEST_BUFFER_SIZE);
}
// Test context init and destroy functions

void test_context_init(test_context* ctx){
    ctx->buffer = malloc(sizeof(char) * TEST_BUFFER_SIZE);
    if (!ctx->buffer){
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    ctx->buffer[0] = '\0'; // Empty sting init to work with strlcat
};

void test_context_destroy(test_context* ctx){
    free(ctx->buffer);
}

