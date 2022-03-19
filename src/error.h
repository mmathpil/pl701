
#ifndef PL701_ERROR_H
#define PL701_ERROR_H

#include "stdio.h"
#include "stdarg.h"

enum ErrorCode {
    PL701_OK = 0,
    PL701_GENERIC,
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


typedef int (*pl701_err_callback)(int errlvl, int errcode, FILE* stream, const char* msg, va_list args); // error level, error code, message. 

int pl701_raise_error(int errlvl, int errcode, const char* message, ...);
pl701_err_callback pl701_set_error_callback(pl701_err_callback callback); // Return the old callback.

static int pl701_default_err_callback(int errlvl, int errcode, FILE* stream, const char* msg, va_list args);

static void pl701_print_err_color_scheme(int errlvl, FILE* stream);
static void pl701_print_err_code_msg(int errcode, FILE* stream);

// Waring: this marco can only be used in the internal function pl701_print_err_code_msg.
#define PL701_PRINT_ERR_CODE_CASE__(errcode, msg)    case errcode : \
                                                     fprintf(stream, msg); return;\

#define PL701_TRACEC(errcode, msg, ...) { pl701_raise_error(PL701_TRACE, errcode, msg, ##__VA_ARGS__ ); };
#define PL701_TRACE(msg, ...) PL701_TRACEC(PL701_GENERIC, msg, ##__VA_ARGS__ )

#define PL701_INFOC(errcode, msg, ...) { pl701_raise_error(PL701_INFO, errcode, msg, ##__VA_ARGS__ ); };
#define PL701_INFO(msg, ...) PL701_INFOC(PL701_GENERIC, msg, ##__VA_ARGS__ )

#define PL701_WARNC(errcode, msg, ...) { pl701_raise_error(PL701_WARN, errcode, msg, ##__VA_ARGS__ ); };
#define PL701_WARN(msg, ...) PL701_WARNC(PL701_GENERIC, msg, ##__VA_ARGS__ )

#define PL701_ERRORC(errcode, msg, ...) { pl701_raise_error(PL701_ERROR, errcode, msg, ##__VA_ARGS__ ); };
#define PL701_ERROR(msg, ...) PL701_ERRORC(PL701_GENERIC, msg, ##__VA_ARGS__ )

#define PL701_CRITICALC(errcode, msg, ...) { pl701_raise_error(PL701_CRITICAL, errcode, msg, ##__VA_ARGS__ ); };
#define PL701_CRITICAL(msg, ...) PL701_CRITICALC(PL701_GENERIC, msg, ##__VA_ARGS__ )

#define PL701_ASSERTC(x, errcode, msg, ...) if(!x){ PL701_ERRORC(errcode, msg, ##__VA_ARGS__ ) }; 
#define PL701_ASSERT(x, msg, ...) if(!x){ PL701_ERROR(msg,  ##__VA_ARGS__ ); __builtin_trap(); };                                                     
#endif