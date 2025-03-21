//
// Created by Per Hüpenbecker on 13.11.24.
//

#ifndef SIMPLELINEARREGRESSION_CSV_H
#define SIMPLELINEARREGRESSION_CSV_H

#define STANDARD_DELIMITER ','
#define STANDARD_HAS_HEADER true
#define STANDARD_LINE_BUFFER_SIZE 1024

#include <stdbool.h>
#include <stdio.h>
#include "result.h"

// Definition of the csv_callback_t struct to store the csv callback functions.
// The context attribute allows for passing additional data to the callback functions e.g.
// a data structure to fill.
// Clear separation of the callback functions allows clear responsibilities and a clean interface.
// The callback functions are called for each line of the csv file.

typedef struct {

    Result (*csv_callback_data)(char** token,size_t token_count, void* context);
    //bool (*csv_callback_header)(const char* line,size_t token_count, void* context);
    void* context;

} csv_callback_t;

// Definition of the csv_parser_args_t struct to store the csv parsing arguments.

typedef struct {

    bool has_header;
    char delimiter;
    size_t column_count;

} csv_parser_args_t;

typedef struct {

        char delimiter;
        bool debug_info;
        bool has_header;
        char* line_buffer;
        char** token_pointers;
        size_t column_count;
        size_t line_buffer_size;
        size_t line_length;
        size_t line_count;
        char** column_names;

} csv_parser_t;

typedef enum{

    NO_QUOTE,
    QUOTED_VALUE,

} QuoteFlag;

Result csv_parser_t_init_std(csv_parser_t*);

Result csv_parser_t_destroy(csv_parser_t*);

Result csv_parser_parse(const char*, csv_parser_t*, csv_callback_t);

#endif //SIMPLELINEARREGRESSION_CSV_H
