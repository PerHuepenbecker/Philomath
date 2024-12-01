//
// Created by Per Hüpenbecker on 01.12.24.
//

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"
#include "../source/dataset.h"
#include "../source/preprocessor.h"

size_t passed_tests = 0;
size_t failed_tests = 0;

int main () {

    preprocessor_t preprocessor;
    preprocessor_t_init(&preprocessor, 2);

    data_point_test_t* test_array = generate_test_dataset(TEST_REGULAR);
    dataset_t dataset;
    dataset_t_init(&dataset, 2);
    Result res;

    for (int i = 0; i < 12; ++i) {
        res = dataset_t_push_data_point(&dataset, (double[]){test_array[i].x, test_array[i].y}, 2);
        if (!res.is_ok) {
            fprintf(stderr, "Error pushing data point: %s\n", get_error(res).message);
            return 1;
        }
        RUN_TEST(res.is_ok, "Pushing data point to dataset");
    }


    RUN_TEST(preprocessor_t_fit(&preprocessor, &dataset).is_ok, "Dataset fitted");
    RUN_TEST(preprocessor_t_transform(&preprocessor, &dataset).is_ok, "Dataset standardized");

    RUN_TEST(preprocessor.state == DATASET_STANDARDIZED, "Dataset state correct");

    preprocessor_t_unnormalize(&preprocessor, &dataset);

    RUN_TEST(preprocessor.state == DATASET_FITTED, "Dataset unnormalized");

    dataset_t_destroy(&dataset);
    preprocessor_t_destroy(&preprocessor);
    free(test_array);

    return (failed_tests > 0)? 1 : 0;
}