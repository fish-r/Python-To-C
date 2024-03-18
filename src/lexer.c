#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_null.h>

const char *pythonKeywordsList[] = {
    "and",    "as",     "assert", "break",   "class", "def",  "del",    "elif",
    "else",   "except", "exec",   "finally", "for",   "from", "global", "if",
    "import", "in",     "is",     "lambda",  "not",   "or",   "pass",   "print",
    "raise",  "return", "try",    "while",   "with",  "yield"};

const char *pythonPunctList[] = {
    ".",   ";",   ":",  "!",  "?",   "/",  "\\", ",",  "@",  "$",
    "&",   ")",   "(",  "\"", "#",   "[",  "]",  "{",  "}",  "=",
    "+=",  "-=",  "*=", "/=", "//=", "%=", "&=", "|=", "^=", ">>=",
    "<<=", "**=", "+",  "-",  "==",  "\n", "\t", NULL};

Token *create_token(PythonTokenType type, const char *lexeme, int line_number) {
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->lexeme = strdup(lexeme);
  token->line_number = line_number;
  return token;
}

void free_token(Token *token) {
  free(token->lexeme);
  free(token);
}

PythonTokenType is_python_keyword(const char *lexeme) {
  int i;
  for (i = 0; i < numPythonKeywords; i++) {
    if (strcmp(lexeme, pythonKeywordsList[i]) == 0) {
      return i;
    }
  }

  return UNKNOWN;
}

int lex(char *source_code) {
  size_t i;
  PythonTokenType candidate_token_type = UNKNOWN;
  size_t candidate_match_length;
  Token **token_stream = NULL;
  size_t token_count = 0;

  size_t source_code_length = strlen(source_code);
  size_t current_position = 0;
  int current_line_number = 1;

  PythonTokenType token_type = UNKNOWN;
  size_t longest_match = 0;
  char matched_lexeme[256] = {0};
  char candidate_lexeme[256] = {0};

  while (current_position < source_code_length) {
    token_type = UNKNOWN;
    longest_match = 0;
    memset(matched_lexeme, 0, sizeof(matched_lexeme));

    for (i = current_position; i < source_code_length; ++i) {
      memset(candidate_lexeme, 0, sizeof(candidate_lexeme));
      memcpy(candidate_lexeme, source_code + current_position,
             i - current_position + 1);
      candidate_match_length = i - current_position + 1;

      if ((candidate_token_type = is_python_keyword(candidate_lexeme)) !=
          UNKNOWN) {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        strcpy(matched_lexeme, candidate_lexeme);
      }
    }

    if (token_type != UNKNOWN) {
      Token *token =
          create_token(token_type, matched_lexeme, current_line_number);
      token_stream =
          (Token **)realloc(token_stream, (token_count + 1) * sizeof(Token *));
      token_stream[token_count] = token;
      token_count++;
      printf("Token { type: %d, lexeme: '%s', line: '%d'}\n", token->type,
             token->lexeme, token->line_number);
      current_position += longest_match;
    } else {
      if (source_code[current_position] == '\n') {
        current_line_number++;
      }
      if (source_code[current_position] != ' ' &&
          source_code[current_position] != '\n' &&
          source_code[current_position] != '\t' &&
          source_code[current_position] != '\r') {
        printf("Error: Unrecognized character '%c' at line %d\n",
               source_code[current_position], current_line_number);
      }
      ++current_position;
    }
  }

  for (i = 0; i < token_count; i++) {
    free_token(token_stream[i]);
  }
  free(token_stream);

  free(source_code);

  return 0;
}
