//
// Created by Per Hüpenbecker on 22.11.24.
//

#include "../source/linreg.h"
#include "../source/dataset.h"
#include "../source/csv.h"

size_t passed_tests = 0;
size_t failed_tests = 0;

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
        } while (0);                                 \
                                                    \



// Test context to store the buffer for the test

typedef struct{
    char* buffer;
} test_context;

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

// Helper function to write example csv files

int write_example_files(){
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

    return 0;
}

// Test callback function to concatenate the tokens into the buffer

Result test_1_callback(char** token, size_t token_count, void* context) {
    test_context* ctx = (test_context*)context;

    for (int i = 0; i < token_count; ++i) {
        if (token[i] == NULL){
            strlcat(ctx->buffer, "(NULL)", TEST_BUFFER_SIZE);
        } else {
            strlcat(ctx->buffer, token[i], TEST_BUFFER_SIZE);
        }
        if (i<token_count-1){
            strlcat(ctx->buffer, "-", TEST_BUFFER_SIZE);
        }
    }
    strlcat(ctx->buffer, "\n", TEST_BUFFER_SIZE);
    return Ok(VOID);
}

// Cleanup function to remove the test files

void cleanup(){
    remove("test_1.csv");
    remove("test_2.csv");
    remove("test_3.csv");
}

int main() {

    // Write the test files

    write_example_files();

    // Initialize the parser and the test context

    csv_parser_t test_parser;

    if(!csv_parser_t_init_std(&test_parser).is_ok){
        return 1;
    }

    test_context ctx;
    test_context_init(&ctx);

    csv_callback_t test_callback = {
        .csv_callback_data = test_1_callback,
        .context = &ctx
    };

    // Parse the test files and check the results

    RUN_TEST(csv_parser_parse("test_1.csv", &test_parser, test_callback).is_ok, "Parsing regular csv");
    RUN_TEST(strcmp(test_parser.column_names[0], "test_x") == 0 && strcmp(test_parser.column_names[1], "test_y") == 0, "Column names parsed correctly");
    RUN_TEST(test_parser.column_count == 2, "Column count parsed correctly");
    RUN_TEST(strcmp(ctx.buffer, "123.4-234.5\n345.6-456.7\n567.8-678.9\n") == 0, "Parsing csv_1 result correct");


    test_context_clear(&ctx);

    RUN_TEST(csv_parser_parse("test_2.csv", &test_parser, test_callback).is_ok, "Parsing csv with missing value");
    RUN_TEST(strcmp(test_parser.column_names[0], "test_x") == 0 && strcmp(test_parser.column_names[1], "test_y") == 0, "Column names parsed correctly");
    RUN_TEST(test_parser.column_count == 2, "Column count parsed correctly");
    RUN_TEST(strcmp(ctx.buffer, "123.4-234.5\n234.5-(NULL)\n") == 0, "Parsing csv_2 result correct");

    test_context_clear(&ctx);

    RUN_TEST(csv_parser_parse("test_3.csv", &test_parser, test_callback).is_ok, "Parsing csv with quoted values");
    RUN_TEST(strcmp(test_parser.column_names[0], "test_x") == 0 && strcmp(test_parser.column_names[1], "test_y") == 0, "Column names parsed correctly");
    RUN_TEST(test_parser.column_count == 2, "Column count parsed correctly");
    RUN_TEST(strcmp(ctx.buffer, "\"Hello, World!\"-HelloWorld!\n") == 0, "Parsing csv_3 result correct");

    test_context_clear(&ctx);

    RUN_TEST(csv_parser_parse("test_4.csv", &test_parser, test_callback).is_ok, "Parsing csv with header only");
    RUN_TEST(strcmp(test_parser.column_names[0], "test_x") == 0 && strcmp(test_parser.column_names[1], "test_y") == 0, "Column names parsed correctly");
    RUN_TEST(test_parser.column_count == 2, "Column count parsed correctly");
    RUN_TEST(strcmp(ctx.buffer, "") == 0, "Parsing csv_4 result correct");

    test_context_destroy(&ctx);

    // remove the test files

    cleanup();

    return (failed_tests > 0)? 1 : 0;
}