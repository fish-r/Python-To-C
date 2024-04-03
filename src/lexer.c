#include "../include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

const char *pythonKeywordsList[] = {
    "and", "as", "assert", "break", "class", "def", "del", "elif",
    "else", "except", "exec", "finally", "for", "from", "global", "if",
    "import", "in", "is", "lambda", "not", "or", "pass", "print",
    "raise", "return", "try", "while", "with", "yield"};

const char *pythonOperatorsList[] = {"+", "-", "*", "**", "/", "//", "%",
                                     "@", "<<", ">>", "&", "|", "^", "~",
                                     ":=", "<", ">", "<=", ">=", "==", "!="};

const char *pythonDelimitersList[] = {
    "(", ")", "[", "]", "{", "}", ",", ":", ".",
    ";", "@", "=", "+=", "-=", "*=", "/=", "//", "%=",
    "@=", "&=", "|=", "^=", ">>", "<<", "**="};

const char *specialPythonTokensList[] = {"'", "\"", "#", "\\"};
const char *unusedPythonTokensList[] = {"$", "?", "`"};

typedef enum
{
  START,
  WORD,
  REJECT
} Identifier_states;

/*
Python token priority:
1. Keywords
2. Operators
3. Literals
4. Identifiers
5. Punctuation
*/

PythonTokenType is_python_keyword(const char *lexeme)
{
  int i;
  for (i = 0; i < numPythonKeywords; i++)
  {
    if (strcmp(lexeme, pythonKeywordsList[i]) == 0)
    {
      return i;
    }
  }

  return UNKNOWN;
}

PythonTokenType is_python_operator(const char *lexeme)
{
  int i;
  for (i = 0; i < numPythonOperators; i++)
  {
    if (strcmp(lexeme, pythonOperatorsList[i]) == 0)
    {
      return i + numPythonKeywords;
    }
  }
  return UNKNOWN;
}

PythonTokenType is_python_delimiter(const char *lexeme)
{
  int i;
  for (i = 0; i < numPythonDelimiters; i++)
  {
    if (strcmp(lexeme, pythonDelimitersList[i]) == 0)
    {
      return i + numPythonKeywords + numPythonOperators;
    }
  }
  return UNKNOWN;
}

PythonTokenType is_python_special(const char *lexeme)
{
  int i;
  for (i = 0; i < numSpecialPythonTokens; i++)
  {
    if (strcmp(lexeme, pythonDelimitersList[i]) == 0)
    {
      return i + numPythonKeywords + numPythonOperators + numPythonDelimiters;
    }
  }
  return UNKNOWN;
}

int is_alpha(char c)
{
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
  {
    return 1; /*Character is an alphabet*/
  }
  else
  {
    return 0; /* Character is not an alphabet*/
  }
}

int is_alphanumeric(char c)
{
  /* Check if the character is between 'A' and 'Z', 'a' and 'z', or '0' and '9'*/
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
  {
    return 1; /* Character is alphanumeric*/
  }
  else
  {
    return 0; /*Character is not alphanumeric*/
  }
}

int is_digit(char c)
{
  return c >= '0' && c <= '9';
}

PythonTokenType is_python_identifier(const char *lexeme)
{
  int i;
  Identifier_states state = START;

  /* check if word is part of python keyword */
  for (i = 0; i < numPythonKeywords; i++)
  {
    if (strcmp(lexeme, pythonKeywordsList[i]) == 0)
    {
      return i;
    }
  }

  while (*lexeme)
  {
    switch (state)
    {
    case START:
      if (is_alpha(*lexeme) || *lexeme == '_')
      {
        state = WORD;
      }
      else
      {
        state = REJECT;
      }
      break;

    case WORD:
      if (!is_alphanumeric(*lexeme) && *lexeme != '_')
      {
        state = REJECT;
      }
      break;
    case REJECT:
      break;
    }

    lexeme++;
  }
  return (state == WORD) ? PYTOK_IDENTIFIER : UNKNOWN;
}

PythonTokenType is_python_string(const char *lexeme, size_t *matched_length)
{
  char quote = *lexeme;
  size_t length = 0;
  /* check if it starts with ' or " , then stop when it encounters the same quote*/
  if (quote == '\'' || quote == '"')
  {
    const char *ptr = lexeme + 1;
    length++;
    while (*ptr != quote && *ptr != '\0')
    {
      ptr++;
      length++;
    }
    if (*ptr == quote)
    {
      length++;
      *matched_length = length;
      return PYTOK_STRING;
    }
  }
  *matched_length = 0;
  return UNKNOWN;
}

PythonTokenType is_python_numeric(const char *lexeme, size_t *matched_length)
{
  size_t length = 0;
  int decimal_point_count = 0;
  int has_digits = 0;

  /* Check if the first character is a digit or a minus sign */
  if (*lexeme == '-' || is_digit(*lexeme))
  {
    const char *ptr = lexeme;
    if (*ptr == '-')
      ptr++;
    while (is_digit(*ptr) || *ptr == '.')
    {
      if (*ptr == '.')
      {
        decimal_point_count++;
        if (decimal_point_count > 1)
        {
          *matched_length = 0;
          return UNKNOWN;
        }
      }
      else
      {
        has_digits = 1;
      }
      ptr++;
      length++;
    }

    if (length > 0)
    {
      *matched_length = length;
      if (decimal_point_count == 0)
      {
        return (has_digits ? PYTOK_INT : UNKNOWN);
      }
      else
      {
        return PYTOK_FLOAT;
      }
    }
  }

  *matched_length = 0;
  return UNKNOWN;
}

PythonTokenType is_python_boolean(const char *lexeme)
{
  if (strcmp(lexeme, "True") == 0 || strcmp(lexeme, "False") == 0)
  {
    return PYTOK_BOOLEAN;
  }
  return UNKNOWN;
}

PythonTokenType is_python_none(const char *lexeme)
{
  if (strcmp(lexeme, "None") == 0)
  {
    return PYTOK_NONE;
  }
  return UNKNOWN;
}

int lex(char *source_code)
{
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

  while (current_position < source_code_length)
  {
    token_type = UNKNOWN;
    longest_match = 0;
    memset(matched_lexeme, 0, sizeof(matched_lexeme));

    for (i = current_position; i < source_code_length; ++i)
    {
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
              UNKNOWN ||
          (candidate_token_type = is_python_boolean(candidate_lexeme)) !=
              UNKNOWN ||
          (candidate_token_type = is_python_none(candidate_lexeme)) !=
              UNKNOWN ||
          (candidate_token_type = is_python_identifier(candidate_lexeme)) !=
              UNKNOWN)
      {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        strcpy(matched_lexeme, candidate_lexeme);
      }
      else if ((candidate_token_type = is_python_numeric(candidate_lexeme, &candidate_match_length)) != UNKNOWN)
      {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        strncpy(matched_lexeme, candidate_lexeme, candidate_match_length);
      }
      else if ((candidate_token_type = is_python_string(candidate_lexeme, &candidate_match_length)) !=
               UNKNOWN)
      {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        strncpy(matched_lexeme, candidate_lexeme, candidate_match_length);
      }
    }

    if (token_type != UNKNOWN)
    {
      Token *token =
          create_token(token_type, matched_lexeme, current_line_number);
      token_stream =
          (Token **)realloc(token_stream, (token_count + 1) * sizeof(Token *));
      token_stream[token_count] = token;
      token_count++;
      printf("Token { type: %d, lexeme: '%s', line: '%d'}\n", token->type,
             token->lexeme, token->line_number);
      current_position += longest_match;
    }
    else
    {
      if (source_code[current_position] == '\n')
      {
        current_line_number++;
      }
      if (source_code[current_position] != ' ' &&
          source_code[current_position] != '\n' &&
          source_code[current_position] != '\t' &&
          source_code[current_position] != '\r')
      {
        printf("Error: Unrecognized character '%c' at line %d\n",
               source_code[current_position], current_line_number);
      }
      ++current_position;
    }
  }

  for (i = 0; i < token_count; i++)
  {
    free_token(token_stream[i]);
  }
  free(token_stream);

  free(source_code);

  return 0;
}

Token *create_token(PythonTokenType type, const char *lexeme, int line_number)
{
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->lexeme = strdup(lexeme);
  token->line_number = line_number;
  return token;
}

void free_token(Token *token)
{
  free(token->lexeme);
  free(token);
}
