//
// Created by Per Hüpenbecker on 13.11.24.
//

#include "csv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int csv_parser_t_init_std(csv_parser_t* parser){
    if (!parser){
        return -1;
    }
    parser->delimiter = STANDARD_DELIMITER;
    parser->has_header = STANDARD_HAS_HEADER;
    parser->line_buffer = malloc(sizeof(char) * STANDARD_LINE_BUFFER_SIZE);
    if (!parser->line_buffer){
        return -1;
    }
    parser->token_buffer = NULL;
    parser->column_count = 0;
    parser->line_buffer_size = STANDARD_LINE_BUFFER_SIZE;
    parser->line_length = 0;
    parser->line_count = 0;
    parser->column_names = NULL;

    return 0;
};

int csv_parser_t_destroy(csv_parser_t* parser){
    if(!parser){
        return -1;
    }
    free(parser->line_buffer);
    if(parser->token_buffer){
        for (size_t i = 0; i <parser->column_count; i++){
            free(parser->token_buffer[i]);
        }
        free(parser->token_buffer);
    }
    if(parser->column_names){
        for (size_t i = 0; i < parser->column_count; i++){
            free(parser->column_names[i]);
        }
        free(parser->column_names);
    }
    return 0;
}

static int csv_parser_token_buffer_init(csv_parser_t* parser) {
    if (!parser || !parser->line_buffer || parser->line_buffer_size == 0) {
        return -1;
    }

    QuoteFlag quoteFlag = NO_QUOTE;
    size_t token_count_new = 0;

    for (int i = 0; i < strlen(parser->line_buffer) - 1; ++i) {
        if (parser->line_buffer[i] == '"') {
            switch (parser->line_buffer[i + 1]) {
                case '"':
                    quoteFlag = QUOTED_VALUE;
                    i += 1;
                    break;
                default:
                    if (quoteFlag == NO_QUOTE) {
                        quoteFlag = QUOTED_VALUE;
                    } else {
                        quoteFlag = NO_QUOTE;
                    }
                    break;
            }
        } else if (parser->line_buffer[i] == ',' && quoteFlag == NO_QUOTE) {
            token_count_new += 1;
        }

    }

    parser->column_count = token_count_new + 1;

    parser->token_buffer = malloc(sizeof(char *) * (parser->column_count));
    if (!parser->token_buffer) {
        return -1;
    }
    for (size_t i = 0; i < parser->column_count; ++i) {
        (parser->token_buffer)[i] = (char*) malloc(sizeof(char) * STANDARD_LINE_BUFFER_SIZE);
        if (!(parser->token_buffer)[i]) {
            return -1;
        }
    }

    return 0;
}

static int csv_parser_tokenize_and_trim_line(char* line, char delimiter, char** token, size_t* token_count){
    QuoteFlag quoteFlag = NO_QUOTE;

    for (int i = 0; i < strlen(line)-1; i++){
        if (line[i] == '"'){
            switch (line[i+1]) {
                case '"':
                    quoteFlag = QUOTED_VALUE;
                    line[i] = '\a';
                    i += 1;
                    break;
            }
        }
    }
};

int read_csv(const char* file_path, csv_parser_t* parser, csv_callback_t callback){

    if (file_path == NULL || parser == NULL || callback.csv_callback == NULL){
        return -1;
    }
    FILE* file = fopen(file_path, "r");

    if(!file) {
        return -1;
    }

    if (parser->has_header){
        if (getline(&line_buffer, &buffer_size, file) == -1){
            return -1;
        }
    }







    free(line_buffer);
    return 0;
}