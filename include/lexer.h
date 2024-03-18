#ifndef LEXER_H
#define LEXER_H
#include "python_tokens.h"

typedef struct {
  PythonTokenType type;
  char *lexeme;
  int line_number;
} Token;

Token *create_token(PythonTokenType type, const char *lexeme, int line_number);
void free_token(Token *token);
int lex(char *source_code);

#endif
