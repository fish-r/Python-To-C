#ifndef LITERAL_FSM_H
#define LITERAL_FSM_H
#include <stddef.h>
#include "python_tokens.h"

typedef enum {
    START_STR_STATE,
    SINGLE_QUOTE_STATE,
    DOUBLE_QUOTE_STATE,
    ESCAPE_STATE,
    END_STR_STATE,
    UNKNOWN_STR_STATE
} Str_State;

typedef enum {
    START_INT_STATE,
    INTEGER_STATE,
    DECIMAL_STATE,
    END_INT_STATE,
    UNKNOWN_INT_STATE
} Int_State;

typedef enum {
    START_LIST_STATE,
    IN_ELEMENT_STATE,
    IN_STRING_STATE,
    END_LIST_STATE,
    UNKNOWN_LIST_STATE
} List_State;

PythonTokenType is_python_string(char *lexeme, size_t *matched_length);
PythonTokenType is_python_numeric(const char *lexeme, size_t *matched_length);
PythonTokenType is_python_list(const char *lexeme, size_t *matched_length, int *list_length);

#endif
