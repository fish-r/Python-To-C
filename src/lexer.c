#include "../include/lexer.h"
#include "../include/utils.h"
#include "../include/literal_fsm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

const char *pythonKeywordsList[] = {
    "and", "as", "assert", "break", "class", "def", "del", "elif",
    "else", "except", "exec", "finally", "for", "from", "global", "if",
    "import", "in", "is", "lambda", "not", "or", "pass", "print",
    "raise", "return", "try", "while", "with", "yield", "range"};

const char *pythonOperatorsList[] = {"+", "-", "*", "**", "/", "//", "%",
                                     "@", "<<", ">>", "&", "|", "^", "~",
                                     ":=", "<", ">", "<=", ">=", "==", "!="};

const char *pythonDelimitersList[] = {
    "(", ")", "{", "}", ",", ":", ".",
    ";", "@", "=", "+=", "-=", "*=", "/=", "//", "%=",
    "@=", "&=", "|=", "^=", ">>", "<<", "**="};

const char *specialPythonTokensList[] = {"'", "\"", "\\"};
const char *unusedPythonTokensList[] = {"$", "?", "`"};

/* Unimplemented Keywords List*/
const char *unImplementedKeywordsTokenList[] = {
    "and", "as", "assert", "class", "del", "except",
    "exec", "finally", "from", "global", "import",
    "is", "lambda", "not", "or", "pass", "raise",
    "try", "with", "yield"};
const char *unImplementedOperatorsTokenList[] = {"**", "//", "@", ":="};
const char *unImplementedDelimitersTokenList[] = {"{", "}", "//", "@=", "**="};

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

PythonTokenType is_python_comment(const char *lexeme, size_t *matched_length)
{
  char comment_symbol = *lexeme;
  size_t length = 0;

  if (comment_symbol == '#')
  {
    const char *ptr = lexeme + 1;
    length++;

    /* continue until end of line or encounter a newline character */
    while (*ptr != '\0' && *ptr != '\n')
    {
      ptr++;
      length++;
    }

    if (*ptr == '\n' || *ptr == '\0')
    {
      *matched_length = length;
      return PYTOK_COMMENT;
    }
  }

  *matched_length = 0;
  return UNKNOWN;
}
PythonTokenType is_python_multi_comment(const char *lexeme, size_t *matched_length)
{
  /* Check if it starts with a multi-line comment symbol*/
  if (strncmp(lexeme, "\"\"\"", 3) == 0 || strncmp(lexeme, "'''", 3) == 0)
  {
    const char *ptr = lexeme + 3;
    size_t length = 3;

    /* continue until end of the multi-line comment symbol or the end of the string */
    while (*ptr != '\0' && !(strncmp(ptr, "\"\"\"", 3) == 0 || strncmp(ptr, "'''", 3) == 0))
    {
      ptr++;
      length++;
    }

    if (*ptr != '\0')
    {
      length += 3;
      *matched_length = length;
      return PYTOK_MULTI_COMMENT;
    }
  }

  *matched_length = 0;
  return UNKNOWN;
}

Token **lex(char *source_code)
{
  size_t i;
  PythonTokenType candidate_token_type = UNKNOWN;
  size_t candidate_match_length;
  Token **token_stream = NULL;
  size_t token_count = 0;
  Token *final_token;

  size_t source_code_length = strlen(source_code);
  size_t current_position = 0;
  int current_line_number = 1;
  int current_indentation = 0;
  char *c_type = NULL;
  int str_length = 0;
  int list_length = 0;

  PythonTokenType token_type = UNKNOWN;
  size_t longest_match = 0;
  char matched_lexeme[256] = {0};
  char candidate_lexeme[256] = {0};

  while (current_position < source_code_length)
  {
    c_type = NULL;
    token_type = UNKNOWN;
    longest_match = 0;
    str_length = 0;
    list_length = 0;
    memset(matched_lexeme, 0, sizeof(matched_lexeme));

    /* Check indentation at the beginning of the line*/
    if (source_code[current_position] == '\n')
    {
      Token *eol_token = create_token(PYTOK_EOL, "EOL", current_line_number, current_indentation, "EOL", str_length);
      token_stream = (Token **)realloc(token_stream, (token_count + 1) * sizeof(Token *));
      token_stream[token_count] = eol_token;
      token_count++;
      printf("Token { type: EOL, lexeme: 'PYTOK_EOL', line: '%d', num_indentation: '%d', c_type: '%s', str_length: '%d'}\n", current_line_number, current_indentation, "EOL", str_length);

      current_indentation = 0;
      ++current_line_number;
      ++current_position;
      continue;
    }

    /*Check if the character is a space and current indentation is 0 (start of line) */
    /* ASSUME first line has 0 indentation*/
    if (source_code[current_position - 1] != '\n')
    {
      current_indentation = 0;
    }
    if (source_code[current_position] == ' ' &&
        current_indentation == 0 &&
        source_code[current_position - 1] != '\n' &&
        current_line_number != 1)
    {
      ++current_indentation;
      while (source_code[current_position] == ' ')
      {
        ++current_indentation;
        ++current_position;
      }
    }

    /* Reset indentation if it's not a multiple of 4 */
    if (current_indentation % 4 != 0)
    {
      current_indentation = 0;
    }

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
      else if ((candidate_token_type = is_python_numeric(candidate_lexeme, &candidate_match_length)) != UNKNOWN ||
               (candidate_token_type = is_python_multi_comment(candidate_lexeme, &candidate_match_length)) != UNKNOWN ||
               (candidate_token_type = is_python_string(candidate_lexeme, &candidate_match_length)) != UNKNOWN ||
               (candidate_token_type = is_python_comment(candidate_lexeme, &candidate_match_length)) != UNKNOWN)
      {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        /* convert python string of length 1 to char with single quote */
        /* else always double quote*/
        if (token_type==PYTOK_CHAR){
              char *modified_lexeme = malloc(3 * sizeof(char)); 
              modified_lexeme[0] = '\''; 
              modified_lexeme[1] = candidate_lexeme[1]; 
              modified_lexeme[2] = '\''; 
              modified_lexeme[3] = '\0'; 
              strncpy(candidate_lexeme, modified_lexeme,3);
              free(modified_lexeme); 
        if (token_type==PYTOK_STRING)
        {     char *modified_lexeme = malloc((longest_match + 3) * sizeof(char));
              modified_lexeme[0] = '\"'; 
              strncpy(modified_lexeme + 1, candidate_lexeme + 1, longest_match - 2); 
              modified_lexeme[longest_match - 1] = '\"'; 
              modified_lexeme[longest_match] = '\0'; 
              strcpy(candidate_lexeme, modified_lexeme);
              free(modified_lexeme);
          }
        }
        strncpy(matched_lexeme, candidate_lexeme, candidate_match_length);
      }
      else if ((candidate_token_type = is_python_list(candidate_lexeme, &candidate_match_length, &list_length)) != UNKNOWN)
      {
        token_type = candidate_token_type;
        longest_match = candidate_match_length;
        str_length = list_length;
        strncpy(matched_lexeme, candidate_lexeme, candidate_match_length);
      }
    }

    /* Check for error */
    checkAndThrowError(matched_lexeme, unImplementedKeywordsTokenList, numUnImplementedKeywordsTokenList, "Keyword", current_line_number);
    checkAndThrowError(matched_lexeme, unImplementedOperatorsTokenList, numUnImplementedOperatorsTokenList, "Operator", current_line_number);
    checkAndThrowError(matched_lexeme, unImplementedDelimitersTokenList, numUnImplementedDelimitersTokenList, "Delimiter", current_line_number);
    /* Assign c_type */
    switch (token_type)
    {
    case (PYTOK_INT):
      c_type = "int";
      break;
    case (PYTOK_FLOAT):
      c_type = "float";
      break;
    case (PYTOK_CHAR):
      c_type = "char";
      str_length = 1;
      break;
    case (PYTOK_STRING):
      c_type = "char []";
      str_length = longest_match - 2;
      break;
    case (PYTOK_LIST_INT):
      c_type = "arr_int";
      break;
    case (PYTOK_LIST_FLOAT):
      c_type = "arr_float";
      break;
    case (PYTOK_LIST_STR):
      c_type = "arr_str";
      break;
    case (PYTOK_BOOLEAN):
      c_type = "bool";
      break;
    case (PYTOK_COMMENT):
      c_type = "comment";
      str_length = longest_match;
      break;
    case (PYTOK_MULTI_COMMENT):
      c_type = "multi_comment";
      str_length = longest_match;
      break;
    default:
      c_type = NULL;
      str_length = longest_match;
      break;
    }

    if (token_type != UNKNOWN)
    {
      Token *token =
          create_token(token_type, matched_lexeme, current_line_number, current_indentation, c_type, str_length);
      token_stream =
          (Token **)realloc(token_stream, (token_count + 1) * sizeof(Token *));
      token_stream[token_count] = token;
      token_count++;
      printf("Token { type: %d, lexeme: '%s', line: '%d', num_indentation: '%d', c_type: '%s', str_length: '%d'}\n", token->type,
             token->lexeme, token->line_number, token->num_indentation, token->c_type, str_length);
      current_position += longest_match;
    }
    else
    {
      if (source_code[current_position] != ' ' &&
          source_code[current_position] != '\n' &&
          source_code[current_position] != '\t' &&
          source_code[current_position] != '\r')
      {
        throwError("Unimplemented Unknown Token '%s' at line %d\n",
                   matched_lexeme, current_line_number);
      }
      ++current_position;
    }
  }
  final_token = create_token(PYTOK_EOL, "EOL", 0, 0, "EOL", 0);
  token_stream = (Token **)realloc(token_stream, (token_count + 1) * sizeof(Token *));
  token_stream[token_count] = final_token;
  token_count++;
  token_stream[token_count] = create_token(PYTOK_EOF, "EOF", 0, 0, "EOF", str_length);
  return token_stream;
  /* ignore below cos token_stream is needed
  for (i = 0; i < token_count; i++)
  {
    free_token(token_stream[i]);
  }
  free(token_stream);

  free(source_code);

  return 0;
  */
}

Token *create_token(PythonTokenType type, const char *lexeme, int line_number, int num_indentation, char *c_type, int str_length)
{
  Token *token = (Token *)malloc(sizeof(Token));
  token->type = type;
  token->lexeme = strdup(lexeme);
  token->line_number = line_number;
  token->num_indentation = num_indentation;
  token->c_type = c_type;
  token->str_length = str_length;
  return token;
}

void free_token(Token *token)
{
  free(token->lexeme);
  free(token);
}
