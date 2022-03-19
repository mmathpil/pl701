#include "stdio.h"
#include "error.h"


pl701_err_callback pl701_global_error_callback = &pl701_default_err_callback;

pl701_err_callback 
pl701_set_error_callback(pl701_err_callback callback){

    if(callback) {
        pl701_err_callback temp = pl701_global_error_callback;
        pl701_global_error_callback = callback;
        return temp;
    };

    return pl701_global_error_callback;
};


int 
pl701_raise_error(int errlvl, int errcode, const char* message, ...){

    va_list args;
    va_start(args, message);

    if(pl701_global_error_callback){
        return pl701_global_error_callback(errlvl, errcode, stderr, message, args);
    }

    va_end(args);

    return PL701_NO_ERR_CALLBACK;

}


static int pl701_default_err_callback(int errlvl, 
                                      int errcode, 
                                      FILE* stream, 
                                      const char* msg,
                                      va_list args)
{
    pl701_print_err_color_scheme(errlvl, stream);
    pl701_print_err_code_msg(errcode, stream);

    vfprintf(stream, msg, args);


    fprintf(stream, "\x1b[0m");
    fprintf(stream, "\n");


    return errcode;
};


static void 
pl701_print_err_color_scheme(int errlvl, FILE* stream){
    
    switch(errlvl){
        case PL701_TRACE:
            fprintf(stream, "\x1b[38;5;248m"); return; // grey
        case PL701_INFO:
            fprintf(stream, "\x1b[32m"); return; // green
        case PL701_WARN:
            fprintf(stream, "\x1b[33m"); return; // yellow
        case PL701_ERROR:
            fprintf(stream, "\x1b[31m"); return; // red
        case PL701_CRITICAL:
            fprintf(stream, "\x1b[1;31m"); return; // bold red

    }

};


static void pl701_print_err_code_msg(int errcode, FILE* stream){

        switch(errcode){
        case PL701_GENERIC: return;
        PL701_PRINT_ERR_CODE_CASE__(PL701_OK, "[ok]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_MISUSE, "[misused]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_NO_ERR_CALLBACK, "[no error callback]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_FAILED_READ_FILE, "[read file error]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_FAILED_WRITE_FILE, "[write file error]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_FAILED_OPEN_FILE, "[open file error]")
        PL701_PRINT_ERR_CODE_CASE__(PL701_FAILED_CLOSE_FILE, "[close file error]")
        PL701_PRINT_ERR_CODE_CASE__( PL701_EOF_ERROR , "[EOF error]")
        default:
            fprintf(stream , "[%d]", errcode);


    };

    fprintf(stream, " ");

}