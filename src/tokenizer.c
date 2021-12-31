#include "stdlib.h"
#include "string.h"
#include "tokenizer.h"


static int 
pl701_init_tokenizer( Tokenizer ** tokenizer, 
                                const char* src_file){

    FILE* f = fopen(src_file, "rb");

    if(!f) return PL701_FAILED_OPEN_FILE;

    Tokenizer* tkzr = *tokenizer;

    tkzr = (Tokenizer*)malloc(sizeof(Tokenizer));
    tkzr->source_file = f;

    tkzr->current_pos = tkzr->buffer;
    tkzr->foward_pos =  tkzr->buffer;

    tkzr->line_count = 0;
    tkzr->line_pos = 0;

    tkzr->buffer[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;
    tkzr->buffer_back[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;

    tkzr->current_buffer = 0;
    return PL701_OK;

};

static int 
pl701_tokenizer_load_file( Tokenizer * const tokenizer ){

    if(!tokenizer) return PL701_MISUSE;

    FILE* f = tokenizer->source_file;

    uint8_t* bptr = tokenizer->buffer;
    size_t read_sz = fread(bptr, PL701_TOKENIZER_BLOCK_SZ, sizeof(uint8_t), f);

    if(!read_sz) return  PL701_FAILED_READ_FILE;

    if(read_sz < PL701_TOKENIZER_BLOCK_SZ){
        tokenizer->buffer[read_sz] = pl701_EOF;
    };

    return PL701_OK;
};

static int 
pl701_tokenizer_finalized( Tokenizer * const tokenizer ){

    if(!fclose(tokenizer->source_file)) return  PL701_FAILED_CLOSE_FILE;
    free(tokenizer);
    return PL701_OK;

};


static int 
pl701_tokenizer_swap_buffer( Tokenizer * const tokenizer ) {

    uint8_t* bptr = tokenizer->buffer_back;
    FILE* f = tokenizer->source_file;

    tokenizer->foward_pos = bptr;
    tokenizer->current_buffer = 1;

    size_t read_sz = fread(bptr, PL701_TOKENIZER_BLOCK_SZ, sizeof(uint8_t), f);

    if(!read_sz) return  PL701_FAILED_READ_FILE;

    if(read_sz < PL701_TOKENIZER_BLOCK_SZ){
        tokenizer->buffer_back[read_sz] = pl701_EOF;
    };

    return PL701_OK;
};

static int 
pl701_tokenizer_swap_buffer_back( Tokenizer * const tokenizer ) {

    uint8_t* bptr = tokenizer->buffer;
    FILE* f = tokenizer->source_file;

    tokenizer->foward_pos = bptr;
    tokenizer->current_buffer = 0;

    size_t read_sz = fread(bptr, PL701_TOKENIZER_BLOCK_SZ, sizeof(uint8_t), f);

    if(!read_sz) return  PL701_FAILED_READ_FILE;

    if(read_sz < PL701_TOKENIZER_BLOCK_SZ){
        tokenizer->buffer[read_sz] = pl701_EOF;
    };

    return PL701_OK;

};

static int 
pl701_copy_token( Token** token, Tokenizer const * tokenizer,
                  const StatesMask_t mask){

     uint8_t* cpos = tokenizer->current_pos;
     uint8_t* fpos = tokenizer->foward_pos;

     char buffer[PL701_TOKENIZER_BLOCK_SZ];

     int count = 0;
     while(cpos != fpos){
        if(*cpos == pl701_EOB){
            if(tokenizer->current_buffer) {
                cpos = tokenizer->buffer_back;
            } else {
                cpos = tokenizer->buffer;
            }
        }

        buffer[count] = *cpos;
        count++; cpos++;
     };

     TokenTag tag = TK_UNDEFINED;
     pl701_get_tag_from_mask(mask, &tag);
     return pl701_new_token( token, buffer, count, tag);

    };


static int
pl701_new_token( Token ** token, char* name, size_t size, TokenTag tag){
     
     Token* tk = *token;
     tk = (Token*)malloc(sizeof(Token));

     // TODO: should handle unicode in future.
     tk->name = (char*)malloc((size + 1)* sizeof(char));
     memcpy(tk->name, name, size * sizeof(char));
     *(tk->name + size) = '\0'; // Make the string null terminated.
     tk->name_len = size;
     tk->tag = tag;

     return PL701_OK; 

};

static int 
pl701_free_token( Token * token){

    if(!token) return PL701_MISUSE;

    free(token->name);
    free(token);
    return PL701_OK; 
};


static int 
pl701_next_token(Tokenizer * const tokenizer, Token** token, int* sucess){
    
    StatesMask_t mask = 1; // Initial state.

    int line = tokenizer->line_count;
    int lpos = tokenizer->line_pos;

    char ch = *(tokenizer->foward_pos);
    mask = pl701_find_epsilon_closure(mask);

    while(1){

        switch(ch){

        case PL701_TOKENIZER_EOB:

            if(tokenizer->current_buffer) { 
                pl701_tokenizer_swap_buffer_back(tokenizer);
            } else {
                pl701_tokenizer_swap_buffer(tokenizer);
            }
            break;

        case PL701_TOKENIZER_EOF:
            tokenizer->current_pos = tokenizer->foward_pos;
            return PL701_EOF_ERROR; 

        default:
            mask = pl701_query_transition_table(mask, ch);

            if(!mask){ // No states are availble. Invaild token.

                token = NULL;
                *sucess = 0;
                return PL701_OK;

            }else if(mask && pl701_final_states) {
                pl701_copy_token(token, tokenizer, mask);
                *sucess = 1;
                tokenizer->current_pos = tokenizer->foward_pos;
                tokenizer->line_count = line;
                tokenizer->line_pos = lpos;

                return PL701_OK;
            } else {
                break;
            }

        };

        tokenizer->foward_pos++;
        char ch = *(tokenizer->foward_pos);
    
        lpos++;
        if(ch == '\n') {lpos = 0; line++; };
    };
};


static StatesMask_t 
pl701_find_epsilon_closure(const StatesMask_t mask){

    return 0;
};


static StatesMask_t 
pl701_query_transition_table(const StatesMask_t mask,
                            const char ch){

     return PL701_OK;    
};

static void 
pl701_get_tag_from_mask(const StatesMask_t mask, TokenTag* const tag){


};


