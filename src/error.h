
#ifndef PL701_ERROR_H
#define PL701_ERROR_H

#include "stdio.h"

enum ErrorCode {
    PL701_OK = 0,
    PL701_MISUSE,
    PL701_FAILED_READ_FILE,
    PL701_FAILED_WRITE_FILE,
    PL701_FAILED_OPEN_FILE,
    PL701_FAILED_CLOSE_FILE,
    PL701_EOF_ERROR   
};

enum ErrorLevel {
    PL701_TRACE = 0,
    PL701_INFO,
    PL701_WARN,
    PL701_ERROR,
    PL701_CRITICAL   
};


typedef int (*pl701_err_callback)(int errlvl, int errcode, FILE* stream, const char* msg); // error level, error code, message. 

int pl701_raise_error(int errlvl, int errcode, const char* message);
int pl701_set_error_callback(int errlvl, int errcode, FILE* stream, const char* msg);

#endif