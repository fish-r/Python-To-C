#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/lexer.h"
#include "../include/literal_fsm.h"
#include "../include/utils.h"


PythonTokenType is_python_string(char *lexeme, size_t *matched_length) {
    char quote = *lexeme;
    size_t length = 0;
    Str_State currentState = START_STR_STATE;

    while (*lexeme) {
        switch (currentState) {
            case START_STR_STATE:
                if (quote == '\'' || quote == '"') {
                    currentState = (quote == '\'') ? SINGLE_QUOTE_STATE : DOUBLE_QUOTE_STATE;
                    length++;
                } else {
                    currentState = UNKNOWN_STR_STATE;
                }
                break;

            case SINGLE_QUOTE_STATE:
            case DOUBLE_QUOTE_STATE:
                if (*lexeme == quote) {
                    currentState = END_STR_STATE;
                    length++;
                } else if (*lexeme == '\\') {
                    currentState = ESCAPE_STATE;
                    length++;
                } else if (*lexeme != '\0') {
                    length++;
                }
                break;

            case ESCAPE_STATE:
                currentState = (currentState == END_STR_STATE) ? currentState : SINGLE_QUOTE_STATE; 
                length++;
                break;

            case END_STR_STATE:
                *matched_length = length;
                return (length <= 3) ? PYTOK_CHAR : PYTOK_STRING;
            
            case UNKNOWN_STR_STATE:
                *matched_length = 0;
                return UNKNOWN;
        }
        lexeme++;
    }
    /* handle case where string is the last token*/
    if (currentState == END_STR_STATE) {
        *matched_length = length;
        return (length <= 3) ? PYTOK_CHAR : PYTOK_STRING;
    } else {
        *matched_length = 0;
        return UNKNOWN;
    }
}

PythonTokenType is_python_numeric(const char *lexeme, size_t *matched_length) {
    size_t length = 0;
    int decimal_point_count = 0;
    Int_State currentState = START_INT_STATE;

    while ((*lexeme)) {
        switch (currentState) {
            case START_INT_STATE:
                if (*lexeme == '-' || is_digit(*lexeme)) {
                    currentState = INTEGER_STATE;
                    length++;
                } else {
                    currentState = UNKNOWN_INT_STATE;
                }
                break;

            case INTEGER_STATE:
                if (is_digit(*lexeme)) {
                    length++;
                } else if (*lexeme == '.') {
                    currentState = DECIMAL_STATE;
                    length++;
                } else {
                    currentState = END_INT_STATE;
                }
                break;

            case DECIMAL_STATE:
                if (is_digit(*lexeme)) {
                    decimal_point_count++;
                    length++;
                } else {
                    currentState = END_INT_STATE;
                }
                break;

            case END_INT_STATE:
                *matched_length = length;
                if (decimal_point_count == 0) {
                    return PYTOK_INT;
                } else {
                    return PYTOK_FLOAT;
                }
                break;

            case UNKNOWN_INT_STATE:
                *matched_length = 0;
                return UNKNOWN;
        }
        lexeme++;
    }
    /* handle case where numeric is the last token*/
    *matched_length = length;
    if (length==0){
        return UNKNOWN;
    }
    return (decimal_point_count == 0) ? PYTOK_INT : PYTOK_FLOAT;
}

PythonTokenType is_python_list(const char *lexeme, size_t *matched_length, int *list_length) {
    size_t length = 0;
    int in_quotes = 0;
    int is_int_list = 1;
    int is_float_list = 1;
    List_State currentState = START_LIST_STATE;
    *list_length = 0;

    while (*lexeme) {
        switch (currentState) {
            case START_LIST_STATE:
                if (*lexeme == '[') {
                    currentState = IN_ELEMENT_STATE;
                    lexeme++;
                    length++;
                    if (*(lexeme + 1) == ']') /*if empty list*/
                    {
                        *list_length=0;
                        *matched_length = 2;
                        return PYTOK_LIST_INT;
                    }
                }
                else {
                    currentState = UNKNOWN_LIST_STATE;
                }
                break;

            case IN_ELEMENT_STATE:
                if (*lexeme == '"') {
                    is_int_list = 0;     
                    is_float_list = 0;
                    currentState = IN_STRING_STATE;
                    in_quotes = !in_quotes;
                } else if (*lexeme == ',' && !in_quotes) { /*if comma, +1 element*/
                    (*list_length)++;
                } else if (*lexeme == ']' && !in_quotes) {
                    (*list_length)++; /* account for last element */
                    currentState = END_LIST_STATE;
                }
                if (!in_quotes && *lexeme == '.') 
                {
                    is_int_list=0;
                }
                lexeme++;
                length++;
                break;

            case IN_STRING_STATE:
                if (*lexeme == '"') {
                    currentState = IN_ELEMENT_STATE;
                    in_quotes = !in_quotes;
                }
                lexeme++;
                length++;
                break;

            case END_LIST_STATE:
                *matched_length = length;
                if (is_int_list) {
                    return PYTOK_LIST_INT;
                } else if (is_float_list) {
                    return PYTOK_LIST_FLOAT;
                } else {
                    return PYTOK_LIST_STR;
                }
                break;

            case UNKNOWN_LIST_STATE:
                *matched_length = 0;
                return UNKNOWN;
        }
    }

     /* handle case where list is the last token*/
    *matched_length = length;

    if (length==0){
        return UNKNOWN;
    }
    if (is_int_list) {
        return PYTOK_LIST_INT;
    } else if (is_float_list) {
        return PYTOK_LIST_FLOAT;
    } else {
        return PYTOK_LIST_STR;
    }
}
