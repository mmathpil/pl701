#ifndef PL701_TOKENIZER_H
#define PL701_TOKENIZER_H

#include "stdint.h"
#include "stdio.h"
#include "error.h"


#define PL701_TOKENIZER_BLOCK_SZ 512

static uint8_t pl701_EOF = (uint8_t)(-1);



struct Tokenizer {

    uint8_t buffer[PL701_TOKENIZER_BLOCK_SZ + 1];
    uint8_t buffer_back[PL701_TOKENIZER_BLOCK_SZ + 1];
    // One addtional character cell for EOF symbol.

    uint8_t* current_pos;
    uint8_t* foward_pos;

    uint32_t line_count;
    uint32_t line_pos;

    FILE* source_file;
} typedef Tokenizer;



static int pl701_init_tokenizer( Tokenizer ** tokenizer, 
                                 const char* src_file);
static int pl701_tokenizer_load_file( Tokenizer * const tokenizer );
static int pl701_tokenizer_finalized( Tokenizer * const tokenizer );

static int pl701_tokenizer_swap_buffer( Tokenizer * const tokenizer );
static int pl701_tokenizer_swap_buffer_back( Tokenizer * const tokenizer );

enum TokenTag {

    TK_UNDEFINED = 0,
    TK_ID,
    TK_NUM,
    TK_LITERAL

} typedef TokenTag;

struct Token {
    const char* name;
    uint32_t name_len;
    TokenTag tag;
} typedef Token;

typedef uint64_t TokenHash_t; 


static int pl701_new_token( Token ** token, const char* name, TokenTag tag);
static int pl701_free_token( Token * token);

static int pl701_next_token(Tokenizer * const tokenizer, Token* token);
#endif