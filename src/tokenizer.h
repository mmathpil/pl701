#ifndef PL701_TOKENIZER_H
#define PL701_TOKENIZER_H

#include "stdint.h"
#include "stdio.h"
#include "error.h"


#define PL701_TOKENIZER_BLOCK_SZ 16

#define PL701_TOKENIZER_EOF -1
#define PL701_TOKENIZER_EOB  0

static const uint8_t pl701_EOF = (uint8_t)(PL701_TOKENIZER_EOF);
static const uint8_t pl701_EOB = (uint8_t)(PL701_TOKENIZER_EOB); // End of buffer

typedef uint64_t StatesMask_t;

enum TokenizerState {
    TKZR_UNINIT =0,
    TKZR_READY,
    TKZR_SUCCESS,
    TKZR_FAILED, 
    TKZR_EOF
} typedef TokenizerState;

struct Tokenizer {

    // One addtional character cell for EOF symbol.
    uint8_t buffer[PL701_TOKENIZER_BLOCK_SZ + 1];
    uint8_t buffer_back[PL701_TOKENIZER_BLOCK_SZ + 1];
    

    uint8_t* current_pos;
    uint8_t* foward_pos;

    uint32_t line_count;
    uint32_t line_pos;

    int current_buffer; 
    // 0 if foward_pos is in buffer, otherwise it is in buffer_back 

   
    StatesMask_t  parsestate_bf[PL701_TOKENIZER_BLOCK_SZ];
    StatesMask_t* parsestate_ptr;

    TokenizerState tkinzr_state;

    // The default behavior when the buffer is swapped, it automatically loads 
    // more text on the current buffer. But sometimes we don't want that and 
    // when laod_on_swap == 0, no new text is loaded. 
    int load_on_swap;

    FILE* source_file;

} typedef Tokenizer;



int pl701_init_tokenizer( Tokenizer ** tokenizer, 
                                 const char* src_file);

int pl701_tokenizer_finalized( Tokenizer * const tokenizer );

int pl701_tokenizer_load_file( Tokenizer * const tokenizer );

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

int pl701_free_token( Token * token);

int pl701_next_token(Tokenizer * const tokenizer, 
                            Token** token);

static int pl701_rewind_char(Tokenizer* const tokenizer);
static int pl701_next_char(Tokenizer* const tokenizer, char* ch);

// After a final state have reached, rewind the foward pointer and the parsing states 
// to identify the longest vaild token. If a token is found, update the value of success.

static int pl701_rewind_tokenizer(Tokenizer* const tokenizer);

static int pl701_copy_token( Token** token, Tokenizer const * tokenizer);

// After to token have copied, update the line and position count and update the current pointer.
// Also clean all the statebuffer.
static int pl701_update_tokenizer(Tokenizer* const tokenizer);

#define PL701_TK_STATE_COUNT sizeof(StatesMask_t) * 8
#define PL701_TK_CARACTER_COUNT 256

#define PL701_TK_EPSILON 255

enum ParseState{
    TK_ST_INITIAL = 0,
    TK_ST1 =  1,
    TK_ST2  = 2,
    TK_ST3  = 3,
    TK_ST4  = 4,
    TK_ST5  = 5,
    TK_ST6  = 6,
    TK_ST7  = 7,
    TK_ST8  = 8,
    TK_ST9  = 9,
    TK_ST10 = 10

} typedef ParseState;


static StatesMask_t pl701_transition_table__[PL701_TK_STATE_COUNT][PL701_TK_CARACTER_COUNT];

static int pl701_init_transition_table();


#define Pl701_TK_ADD_TBENTRY(state, ch, mask) do{\
     pl701_transition_table__[state][(uint8_t)ch] = mask;\
} while(0);\

#define Pl701_TK_TBEPSILON(state, mask)  Pl701_TK_ADD_TBENTRY(state, PL701_TK_EPSILON, mask)

// Adding a specific state with a range of characters to the transistion tables.
#define Pl701_TK_TBRANGE(state, ch_begin, ch_end, mask) do{\
    for(uint8_t ch = (uint8_t)ch_begin; ch <= (uint8_t)ch_end; ch++){\
        Pl701_TK_ADD_TBENTRY(state, ch, mask)\
};} while(0);\

// Adding a specific state with a list of characters to the transistion tables.
#define Pl701_TK_TBSETS(state, list, mask) do{\
    const char* li = list;\
    for(uint8_t i = 0; i < sizeof(list); i++){\
        Pl701_TK_ADD_TBENTRY(state, *(list + i), mask)\
 };}while (0); \

#define Pl701_TK_MASK(state) (uint64_t)(1 << state)

static StatesMask_t pl701_final_states =  Pl701_TK_MASK(TK_ST6);


static StatesMask_t pl701_find_epsilon_closure(const StatesMask_t mask);
static StatesMask_t pl701_query_transition_table(const StatesMask_t mask,
                                                 const char ch);
static StatesMask_t pl701_transtb_single_entry(ParseState state, int character);


static void pl701_get_tag_from_mask(const StatesMask_t mask, TokenTag* const tag);




#endif