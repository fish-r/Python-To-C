#ifndef MAPPING_H
#define MAPPING_H

#include "lexer.h"

void assert_c(int expression, const char *message);
void as_c(void);
void class_c(void);
void def_c(void);
void elif_c(void);
void exec_c(void);
void from_c(void);
void global_c(void);
void import_c(void);
void in_c(void);
void is_c(void);
void lambda_c(void);
void not_c(void);
void or_c(void);

CToken *convert_to_c_token(PythonToken *inputPythonToken);

#endif 
