#include "../include/c_tokens.h"
#include "../include/lexer.h"
#include "../include/python_tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_null.h>

/* function in C tjjhat has the same functionality as "assert" in Python*/
void assert_c(int expression, const char *message) {
  if (!expression) {
    fprintf(stderr, "Assertion failed: %s\n", message);
    exit(EXIT_FAILURE);
  }
}

/* init func pointer to assert_c */
void (*assert)(int, const char *) = assert_c;

void as_c(void) { printf("as\n"); }
void class_c(void) { printf("class\n"); }
void def_c(void) { printf("def\n"); }
void elif_c(void) { printf("elif\n"); }
void exec_c(void) { printf("exec\n"); }
void from_c(void) { printf("from\n"); }
void global_c(void) { printf("global\n"); }
void import_c(void) { printf("import\n"); }
void in_c(void) { printf("in\n"); }
void is_c(void) { printf("is\n"); }
void lambda_c(void) { printf("lambda\n"); }
void not_c(void) { printf("not\n"); }
void or_c(void) { printf("or\n"); }

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
CToken *convert_to_c_token(PythonToken *inputPythonToken) {
  CToken *ctok = malloc(sizeof(CToken));
  /* Keywords */
  if (inputPythonToken->type == PY_KEYWORD_AS) {
    ctok->type = C_KEYWORD_AS;
    ctok->func = as_c;
  } else if (inputPythonToken->type == PY_KEYWORD_CLASS) {
    ctok->type = C_KEYWORD_CLASS;
    ctok->func = class_c;
  } else if (inputPythonToken->type == PY_KEYWORD_DEF) {
    ctok->type = C_KEYWORD_DEF;
    ctok->func = def_c;
  } else if (inputPythonToken->type == PY_KEYWORD_ELIF) {
    ctok->type = C_KEYWORD_ELIF;
    ctok->func = elif_c;
  } else if (inputPythonToken->type == PY_KEYWORD_EXEC) {
    ctok->type = C_KEYWORD_EXEC;
    ctok->func = exec_c;
  } else if (inputPythonToken->type == PY_KEYWORD_FROM) {
    ctok->type = C_KEYWORD_FROM;
    ctok->func = from_c;
  } else if (inputPythonToken->type == PY_KEYWORD_GLOBAL) {
    ctok->type = C_KEYWORD_GLOBAL;
    ctok->func = global_c;
  } else if (inputPythonToken->type == PY_KEYWORD_IMPORT) {
    ctok->type = C_KEYWORD_IMPORT;
    ctok->func = import_c;
  } else if (inputPythonToken->type == PY_KEYWORD_IN) {
    ctok->type = C_KEYWORD_IN;
    ctok->func = in_c;
  } else {
    ctok->type = C_UNKNOWN;
    ctok->func = NULL;
  }
  return ctok;
}
