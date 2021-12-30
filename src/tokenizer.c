#include "tokenizer.h"
#include "stdlib.h"

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

    tkzr->buffer[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOF;
    tkzr->buffer_back[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOF;

    return PL701_OK;

};

static int 
pl701_tokenizer_load_file( Tokenizer * const tokenizer ){

    if(!tokenizer) return PL701_MISUSE;

    FILE* f = tokenizer->source_file;

    uint8_t* bptr = tokenizer->buffer;
    size_t read_sz = fread(bptr, PL701_TOKENIZER_BLOCK_SZ, sizeof(uint8_t), f);

    if(read_sz < PL701_TOKENIZER_BLOCK_SZ){
        tokenizer->buffer[read_sz + 1] = pl701_EOF;

    } else {
        bptr = tokenizer->buffer_back;
        read_sz = fread(bptr, PL701_TOKENIZER_BLOCK_SZ, sizeof(uint8_t), f);
        tokenizer->buffer_back[read_sz + 1] = pl701_EOF;
    }


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

    return PL701_OK;
};

static int 
pl701_tokenizer_swap_buffer_back( Tokenizer * const tokenizer ) {

    return PL701_OK;

};



static int
pl701_new_token( Token ** token, const char* name, TokenTag tag){
     
     Token* tk = *token;
     tk = (Token*)malloc(sizeof(Token));

     // TODO: should handle unicode in future.
     tk->name = (char*)malloc(sizeof(name));
     tk->name_len = sizeof(name);
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
pl701_next_token(Tokenizer * const tokenizer, Token* token){
    


}