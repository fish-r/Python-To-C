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

const char *pythonOperatorsList[] = {"+",  "-",  "*",  "**", "/",  "//", "%",
                                     "@",  "<<", ">>", "&",  "|",  "^",  "~",
                                     ":=", "<",  ">",  "<=", ">=", "==", "!="};

const char *pythonDelimitersList[] = {
    "(",  ")",  "[",  "]",  "{",  "}",  ",",  ":",  ".",
    ";",  "@",  "=",  "+=", "-=", "*=", "/=", "//", "%=",
    "@=", "&=", "|=", "^=", ">>", "<<", "**="};

const char *specialPythonTokensList[] = {"'", "\"", "#", "\\"};
const char *unusedPythonTokensList[] = {"$", "?", "`"};

/*
Python token priority:
1. Keywords
2. Operators
3. Literals
4. Identifiers
5. Punctuation
*/

PythonTokenType is_python_keyword(const char *lexeme) {
  int i;
  for (i = 0; i < numPythonKeywords; i++) {
    if (strcmp(lexeme, pythonKeywordsList[i]) == 0) {
      return i;
    }
  }

  return UNKNOWN;
}

PythonTokenType is_python_operator(const char *lexeme) {
  int i;
  for (i = 0; i < numPythonOperators; i++) {
    if (strcmp(lexeme, pythonOperatorsList[i]) == 0) {
      return i + numPythonKeywords;
    }
  }
  return UNKNOWN;
}

PythonTokenType is_python_delimiter(const char *lexeme) {
  int i;
  for (i = 0; i < numPythonDelimiters; i++) {
    if (strcmp(lexeme, pythonDelimitersList[i]) == 0) {
      return i + numPythonKeywords + numPythonOperators;
    }
  }
  return UNKNOWN;
}

PythonTokenType is_python_special(const char *lexeme) {
  int i;
  for (i = 0; i < numSpecialPythonTokens; i++) {
    if (strcmp(lexeme, pythonDelimitersList[i]) == 0) {
      return i + numPythonKeywords + numPythonOperators + numPythonDelimiters;
    }
  }
  return UNKNOWN;
}

PythonToken **lex(char *source_code, int *token_count) {
  size_t i;
  size_t candidate_match_length;
  size_t source_code_length;
  size_t current_position;
  int current_line_number;
  PythonToken **token_stream;
  PythonTokenType candidate_token_type;
  PythonTokenType token_type;
  size_t longest_match;
  char matched_lexeme[256] = {0};
  char candidate_lexeme[256] = {0};

  token_stream = NULL;
  candidate_token_type = UNKNOWN;
  *token_count = 0;
  current_position = 0;
  current_line_number = 1;
  source_code_length = strlen(source_code);

  token_type = UNKNOWN;

  longest_match = 0;

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
              UNKNOWN ||
          (candidate_token_type = is_python_operator(candidate_lexeme)) !=
              UNKNOWN ||
          (candidate_token_type = is_python_delimiter(candidate_lexeme)) !=
              UNKNOWN ||
          (candidate_token_type = is_python_special(candidate_lexeme)) !=
              UNKNOWN) {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        strcpy(matched_lexeme, candidate_lexeme);
      }
    }

    if (token_type != UNKNOWN) {
      PythonToken *token =
          create_token(token_type, matched_lexeme, current_line_number);
      token_stream = (PythonToken **)realloc(
          token_stream, (*token_count + 1) * sizeof(PythonToken *));
      token_stream[*token_count] = token;
      (*token_count)++;
      printf("PythonToken { type: %d, lexeme: '%s', line: '%d'}\n", token->type,
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

  free(source_code);

  return token_stream;
}

PythonToken *create_token(PythonTokenType type, const char *lexeme,
                          int line_number) {
  PythonToken *token = (PythonToken *)malloc(sizeof(PythonToken));
  token->type = type;
  token->lexeme = strdup(lexeme);
  token->line_number = line_number;
  return token;
}

void free_token(PythonToken *token) {
  free(token->lexeme);
  free(token);
}
