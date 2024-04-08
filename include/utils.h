#ifndef UTILS_H
# define UTILS_H

typedef enum {
    NO_ERROR,
    FILE_NOT_FOUND,
    MEMORY_ALLOCATION_FAILED,
    INVALID_INPUT
} ErrorCode;

char *read_file(const char *filename);
void throwError(const char *format, ...);
void checkAndThrowError(const char *matched_lexeme, const char **unImplementedTokenList, 
                        int numTokens, const char *tokenType, int current_line_number);

#endif /* UTILS_H */
