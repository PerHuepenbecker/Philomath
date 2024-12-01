//
// Created by Per Hüpenbecker on 01.12.24.
//

#ifndef PHILOMATH_HELPERS_H
#define PHILOMATH_HELPERS_H

#endif //PHILOMATH_HELPERS_H

#define TEST_BUFFER_SIZE 1024
#define RUN_TEST(test_expression, test_name) do {   \
if (test_expression) {                              \
        printf("[PASS] %s\n", test_name);           \
        passed_tests++;                             \
        } else{                                     \
            printf("[FAIL] %s\n", test_name);       \
            failed_tests++;                         \
        }                                           \
                                                    \
        } while (0);                                \
                                                    \

typedef struct{
    double x;
    double y;
} data_point_test_t;

typedef enum {
    TEST_REGULAR,
    TEST_MISSING_VALUES,
    TEST_LINREG_2DIM,
    TEST_LINREG_3DIM,
} TestDatasetType;

// Test context to store the buffer for the test

typedef struct{
    char* buffer;
} test_context;

void test_context_clear(test_context* ctx);
void test_context_init(test_context* ctx);
void test_context_destroy(test_context* ctx);

void write_example_files();
data_point_test_t* generate_test_dataset(TestDatasetType type);