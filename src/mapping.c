#include "../include/c_tokens.h"
#include "../include/lexer.h"
#include "../include/python_tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function in C tjjhat has the same functionality as "assert" in Python
void assert_c(int expression, const char *message) {
  if (!expression) {
    fprintf(stderr, "Assertion failed: %s\n", message);
    exit(EXIT_FAILURE);
  }
}

/* init func pointer to assert_c */
void (*assert)(int, const char *) = assert_c;

void as_c() { printf("as\n"); }
void class_c() { printf("class\n"); }
void def_c() { printf("def\n"); }
void elif_c() { printf("elif\n"); }
void exec_c() { printf("exec\n"); }
void from_c() { printf("from\n"); }
void global_c() { printf("global\n"); }
void import_c() { printf("import\n"); }
void in_c() { printf("in\n"); }
void is_c() { printf("is\n"); }
void lambda_c() { printf("lambda\n"); }
void not_c() { printf("not\n"); }
void or_c() { printf("or\n"); }

/* For reference only */
/*
Does not have direct equivalent in C; will need to derive functions
    as // can try resassignment
    assert // can try making a function
    class // dont try
    def // print a function with void return
    elif // just print else if
    exec // dont try
    from // dont try
    global // dont try
    import // dont try
    in // dont try
    is // dont try
    lambda // dont try
    not just print !!
    or // print ||
    pass // print nothing
    print // print printf
    raise // dont try
    with // dont try
    yield // dont try

*/

/* takes in list of Python Tokens and returns list of C Tokens*/
const CToken *convert_to_c_token(PythonToken inputPythonToken) {
  // create C token from python token
  CToken ctok;
  if (strcmp(keyword, "as") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "as") == 0) {
      ctok.func = as_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "class") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "class") == 0) {
      ctok.func = class_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "def") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "def") == 0) {
      ctok.func = def_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "elif") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "elif") == 0) {
      ctok.func = elif_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "exec") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "exec") == 0) {
      ctok.func = exec_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "from") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "from") == 0) {
      ctok.func = from_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "global") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "global") == 0) {
      ctok.func = global_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "import") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "import") == 0) {
      ctok.func = import_c;
    }
    return &ctok;
  } else if (strcmp(keyword, "in") == 0) {
    ctok.type = KEYWORD;
    if (strcmp(ctok.lexeme, "in") == 0) {
      ctok.func = in_c;
    }
    return &ctok;
  }
  return NULL;
}
