#ifndef LEXER_H
#define LEXER_H
#include "python_tokens.h"

typedef struct
{
  PythonTokenType type;
  char *lexeme;
  int line_number;
  int num_indentation;
} Token;

char *strdup(const char *s);
Token *create_token(PythonTokenType type, const char *lexeme, int line_number, int num_indentation);
void free_token(Token *token);
Token **lex(char *source_code);

#endif
