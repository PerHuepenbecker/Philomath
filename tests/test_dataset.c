//
// Created by Per Hüpenbecker on 01.12.24.
//


#include <stdio.h>
#include "../source/dataset.h"
#include "helpers.h"

size_t passed_tests = 0;
size_t failed_tests = 0;



int main() {

    data_point_test_t* test_array = generate_test_dataset(TEST_LINREG_2DIM);

    dataset_t dataset;
    dataset_t_init(&dataset, 2);
    Result res;

    for (size_t i = 0; i < 11; i++){
        res = dataset_t_push_data_point(&dataset, (double[]){test_array[i].x, test_array[i].y}, 2);
        if (!res.is_ok){
            fprintf(stderr, "Error pushing data point: %s\n", get_error(res).message);
            return 1;
        }
        RUN_TEST(res.is_ok, "Pushing data point to dataset");
    }

    for (size_t i = 0; i < 11; i++){
        RUN_TEST(dataset.data_points[i].line[0] == test_array[i].x, "X value correct");
        RUN_TEST(dataset.data_points[i].line[1] == test_array[i].y, "Y value correct");
    }

    dataset_t_destroy(&dataset);
    free(test_array);

    return (failed_tests > 0)? 1 : 0;
}