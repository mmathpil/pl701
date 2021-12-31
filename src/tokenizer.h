#ifndef PL701_TOKENIZER_H
#define PL701_TOKENIZER_H

#include "stdint.h"
#include "stdio.h"
#include "error.h"


#define PL701_TOKENIZER_BLOCK_SZ 512

#define PL701_TOKENIZER_EOF -1
#define PL701_TOKENIZER_EOB -2

static const uint8_t pl701_EOF = (uint8_t)(PL701_TOKENIZER_EOF);
static const uint8_t pl701_EOB = (uint8_t)(PL701_TOKENIZER_EOB); // End of buffer


struct Tokenizer {

    uint8_t buffer[PL701_TOKENIZER_BLOCK_SZ + 1];
    uint8_t buffer_back[PL701_TOKENIZER_BLOCK_SZ + 1];
    // One addtional character cell for EOF symbol.

    uint8_t* current_pos;
    uint8_t* foward_pos;

    uint32_t line_count;
    uint32_t line_pos;

    int current_buffer; 
    // 0 if foward_pos is in buffer, otherwise it is in buffer_back 

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
    TK_LITERAL,
    TK_SPECIAL_SYMB

} typedef TokenTag;

struct Token {
    char* name;
    uint32_t name_len;
    TokenTag tag;
} typedef Token;

typedef uint64_t TokenHash_t; 


static int pl701_new_token( Token ** token, char* const name, 
                            size_t size, 
                            TokenTag tag);

static int pl701_free_token( Token * token);

static int pl701_next_token(Tokenizer * const tokenizer, 
                            Token** token, 
                            int* success    );


typedef uint64_t StatesMask_t; 
static StatesMask_t pl701_final_states;

static StatesMask_t pl701_find_epsilon_closure(const StatesMask_t mask);
static StatesMask_t pl701_query_transition_table(const StatesMask_t mask,
                                                 const char ch);
static void pl701_get_tag_from_mask(const StatesMask_t mask, TokenTag* const tag);

static int pl701_copy_token( Token** token, Tokenizer const * tokenizer, 
                             const StatesMask_t mask);

#endif