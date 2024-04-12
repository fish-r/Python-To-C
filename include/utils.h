#ifndef UTILS_H
# define UTILS_H

int is_digit(char c);
int is_whitespace(char c);
char *read_file(const char *filename);
void throwError(const char *format, ...);
void checkAndThrowError(const char *matched_lexeme, const char **unImplementedTokenList, 
                        int numTokens, const char *tokenType, int current_line_number);

#endif /* UTILS_H */
