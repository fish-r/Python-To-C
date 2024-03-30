#ifndef LEXER_H
#define LEXER_H
#include "c_tokens.h"
#include "python_tokens.h"

typedef struct {
  PythonTokenType type;
  char *lexeme;
  int line_number;
} PythonToken;

typedef struct {
  CTokenType type;
  char *lexeme;
  char *to_print;
  // function pointer func
  void (*func)(void);
} CToken;

PythonToken *create_token(PythonTokenType type, const char *lexeme, int line_number);
void free_token(PythonToken *token);
int lex(char *source_code);

#endif
