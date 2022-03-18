
#ifndef PL701_ERROR_H
#define PL701_ERROR_H

#include "stdio.h"

enum ErrorCode {
    PL701_OK = 0,
    PL701_MISUSE,
    PL701_NO_ERR_CALLBACK,
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


typedef int (*pl701_err_callback)(int errlvl, int errcode, FILE* stream, const char* msg, ...); // error level, error code, message. 

int pl701_raise_error(int errlvl, int errcode, const char* message);
pl701_err_callback pl701_set_error_callback(pl701_err_callback callback); // Return the old callback.

static int pl701_default_err_callback(int errlvl, int errcode, FILE* stream, const char* msg, ...);

static void pl701_print_err_color_scheme(int errlvl, FILE* stream);
static void pl701_print_err_code_msg(int errcode, FILE* stream);

// Waring: this marco can only be used in the internal function pl701_print_err_code_msg.
#define PL701_PRINT_ERR_CODE_CASE__(errcode, msg)    case errcode : \
                                                     fprintf(stream, msg); return;\

#endif