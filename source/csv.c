//
// Created by Per Hüpenbecker on 13.11.24.
//

#include "csv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Result csv_parser_t_init_std(csv_parser_t* parser){
    parser->delimiter = STANDARD_DELIMITER;
    parser->has_header = STANDARD_HAS_HEADER;
    parser->line_buffer = malloc(sizeof(char) * STANDARD_LINE_BUFFER_SIZE);
    if (!parser->line_buffer){
        return Err(MEMORY_ALLOCATION_ERROR, "Allocation of line buffer failed.", NULL);
    }
    parser->token_pointers = NULL;
    parser->column_count = 0;
    parser->line_buffer_size = STANDARD_LINE_BUFFER_SIZE;
    parser->line_length = 0;
    parser->line_count = 0;
    parser->column_names = NULL;
    parser->debug_info = false;

    return Ok(VOID);
};

Result csv_parser_t_destroy(csv_parser_t* parser){
    if(!parser){
        return Err(INVALID_FUNCTION_ARGUMENT, "NULL parser argument cannot be destroyed", NULL);
    }
    free(parser->line_buffer);
    if(parser->token_pointers){
        free(parser->token_pointers);
    }
    if(parser->column_names){
        for (size_t i = 0; i < parser->column_count; i++){
            free(parser->column_names[i]);
        }
        free(parser->column_names);
    }
    return Ok(VOID);
}

// Handling of frequent quote flag logic to handle quoted values in the csv file.

static QuoteFlag csv_parser_eval_quote_flag(QuoteFlag flag){
    if (flag == NO_QUOTE){
        return QUOTED_VALUE;
    } else {
        return NO_QUOTE;
    }
};

static Result csv_parser_token_buffer_init(csv_parser_t* parser) {

    QuoteFlag quoteFlag = NO_QUOTE;
    size_t token_count_new = 0;

    // This part is duplicated code from the csv_parser_tokenize_and_trim_line function.
    // Decided for this approach instead of a helper function for keeping the logic local to the
    // function and to avoid complex functions with many parameters.

    for (int i = 0; i < strlen(parser->line_buffer); ++i) {
        if (parser->line_buffer[i] == '"') {
            switch (parser->line_buffer[i + 1]) {
                case '"':

                    // Handling of double quotes in quoted values - here as an escape character.

                    quoteFlag = QUOTED_VALUE;
                    i += 1;
                    break;
                default:
                    quoteFlag = csv_parser_eval_quote_flag(quoteFlag);
                    break;
            }
        } else if (parser->line_buffer[i] == ',' && quoteFlag == NO_QUOTE) {
            token_count_new += 1;
        }

    }

    parser->column_count = token_count_new + 1;

    parser->token_pointers = malloc(sizeof(char *) * (parser->column_count));
    if (!parser->token_pointers) {
        return Err(MEMORY_ALLOCATION_ERROR, "token_pointer buffer allocation failed", NULL);
    }
    for (size_t i = 0; i < parser->column_count; ++i) {
        (parser->token_pointers)[i] = NULL;
    }

    return Ok(VOID);
}

static Result csv_parser_tokenize_and_trim_line(csv_parser_t* parser){
    QuoteFlag quoteFlag = NO_QUOTE;
    size_t line_length = strlen(parser->line_buffer);


    // Logic for marking the values in the read line for deletion. Every line gets processed twice - first here for
    // marking with \0 and \a, where \0 divides the identified tokens and \a marks the chars for deletion in the following
    // run. Tokenization logic runs this way with linear complexity.

    for (int i = 0; i < line_length; i++){
        if (parser->line_buffer[i] == '"'){
            parser->line_buffer[i] = '\a';
            switch (parser->line_buffer[i+1]) {
                case '"':
                    quoteFlag = QUOTED_VALUE;
                    i += 1;
                    break;
                default:
                    quoteFlag = csv_parser_eval_quote_flag(quoteFlag);
                    break;
            }
        }
        else if (parser->line_buffer[i] == ',' && quoteFlag == NO_QUOTE){
            parser->line_buffer[i] = '\0';
        } else if ((parser->line_buffer[i] == ' ' || parser->line_buffer[i] == '\n') && quoteFlag == NO_QUOTE){
            parser->line_buffer[i] = '\a';
        }
    }

    // In place tokenization logic. The line_buffer is used as a buffer for the tokenized line to
    // be prepared for further processing in the callback functions. The token pointers are made to
    // point at the beginning of each token in the line_buffer. The '\0' characters are used as delimiters here.
    // The complete line gets processed in one pass.

    size_t token_index = 0;
    size_t char_pos = 0;


    size_t i = 0;

    while (i < line_length && parser->line_buffer[i] == '\a'){
        i++;
    }

    parser->token_pointers[token_index] = &parser->line_buffer[i];

    for (int i = 0; i < line_length; ++i) {
        switch(parser->line_buffer[i]){
            case '\a':
                break;
            case '\0':
                if(token_index >= parser->column_count){
                    break;
                }
                token_index += 1;
                parser->line_buffer[char_pos] = '\0';
                parser->token_pointers[token_index] = &parser->line_buffer[char_pos+1];
                char_pos += 1;

                break;
            default:
                parser->line_buffer[char_pos] = parser->line_buffer[i];
                char_pos += 1;
        }
    }
    parser->line_buffer[char_pos] = '\0';

    return Ok(VOID);
};

Result csv_parser_parse(const char* file_path, csv_parser_t* parser, csv_callback_t callback){

    Result tmp;

    if (file_path == NULL || parser == NULL || callback.csv_callback_data == NULL){
        return Err(INVALID_FUNCTION_ARGUMENT, "Bad arguments to csv parser function - NULL pointers detected.", NULL);
    }
    FILE* file = fopen(file_path, "r");
    if(!file) {
        return Err(FILE_ERROR, "File could not be opened", file_path);
    }

    if (getline(&(parser->line_buffer), &(parser->line_buffer_size), file) == -1){
        return Err(FILE_PARSING_ERROR, "File appears to be empty.", file_path);
    }

    tmp = csv_parser_token_buffer_init(parser);
    if (!tmp.is_ok) {
        return Err(tmp.error.code, tmp.error.message, tmp.error.context);
    }
    tmp = csv_parser_tokenize_and_trim_line(parser);
    if(!tmp.is_ok){
        return Err(tmp.error.code, tmp.error.message, tmp.error.context);
    }

    if (parser->has_header){
       parser->column_names = malloc(sizeof(char*) * parser->column_count);
         if (!parser->column_names){
              return Err(MEMORY_ALLOCATION_ERROR, "Column names buffer could not be allocated", file_path);
         }


         for (size_t i = 0; i < parser->column_count; i++){
                parser->column_names[i] = malloc(sizeof(char) * strlen(parser->token_pointers[i])+1);
                if (!parser->column_names[i]){
                    for (i--;i!=0;i--){
                        free(parser->column_names[i]);
                    }
                    return Err(MEMORY_ALLOCATION_ERROR, "Column name sting could not be allocated", file_path);
                }
                strcpy(parser->column_names[i], parser->token_pointers[i]);
            }
    }

    while (getline(&(parser->line_buffer), &(parser->line_buffer_size), file) != -1){
        csv_parser_tokenize_and_trim_line(parser);
        callback.csv_callback_data(parser->line_buffer, parser->column_count, callback.context);
    }

    fclose(file);

    return Ok(VOID);
}