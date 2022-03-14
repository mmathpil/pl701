#include "stdlib.h"
#include "string.h"
#include "tokenizer.h"


int 
pl701_init_tokenizer( Tokenizer ** tokenizer, 
                                const char* src_file){

    FILE* f = fopen(src_file, "rb");

    if(!f) return PL701_FAILED_OPEN_FILE;

    Tokenizer* tkzr;

    tkzr = (Tokenizer*)malloc(sizeof(Tokenizer));
    tkzr->source_file = f;

    tkzr->current_pos = tkzr->buffer;
    tkzr->foward_pos =  tkzr->buffer;

    tkzr->line_count = 0;
    tkzr->line_pos = 0;

    tkzr->buffer[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;
    tkzr->buffer_back[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;

    tkzr->current_buffer = 0;

    *tokenizer = tkzr;
    return pl701_init_transition_table();

};

static int 
pl701_init_transition_table(){

    for(int i = 0; i < PL701_TK_STATE_COUNT; i++){
        for(int j = 0; j < PL701_TK_CARACTER_COUNT; j++){
             pl701_transition_table__[i][j] = 0;
        }; };

    // A test transition table with regex ([a-z]|[0-9])*abb.

    Pl701_TK_ADD_TBENTRY(TK_ST_INITIAL , PL701_TK_EPSILON , 
        Pl701_TK_MASK(TK_ST1) | Pl701_TK_MASK(TK_ST7));

    Pl701_TK_ADD_TBENTRY(TK_ST1 , PL701_TK_EPSILON , 
        Pl701_TK_MASK(TK_ST2) | Pl701_TK_MASK(TK_ST4)); 

    Pl701_TK_ADD_TBENTRIES(TK_ST2 ,'a' , 'z', 
            Pl701_TK_MASK(TK_ST3)); 

    Pl701_TK_ADD_TBENTRY(TK_ST3,  PL701_TK_EPSILON , 
            Pl701_TK_MASK(TK_ST6));

    Pl701_TK_ADD_TBENTRIES(TK_ST4 ,'0' , '9',
            Pl701_TK_MASK(TK_ST5));

    Pl701_TK_ADD_TBENTRY(TK_ST5,  PL701_TK_EPSILON , 
            Pl701_TK_MASK(TK_ST6));

    Pl701_TK_ADD_TBENTRY(TK_ST6,  PL701_TK_EPSILON , 
            Pl701_TK_MASK(TK_ST1) | Pl701_TK_MASK(TK_ST7));

    Pl701_TK_ADD_TBENTRY(TK_ST7,  'a' , 
            Pl701_TK_MASK(TK_ST8));           
    
    Pl701_TK_ADD_TBENTRY(TK_ST8,  'b' , 
            Pl701_TK_MASK(TK_ST9));   
    
    Pl701_TK_ADD_TBENTRY(TK_ST9,  'b' , 
            Pl701_TK_MASK(TK_ST10));   

    return PL701_OK;
};

int 
pl701_tokenizer_load_file( Tokenizer * const tokenizer ){

    if(!tokenizer) return PL701_MISUSE;

    FILE* f = tokenizer->source_file;

    clearerr(f);

    for(int i = 0; i <  PL701_TOKENIZER_BLOCK_SZ; i++){
        int ch = getc(f);

        if(ch == EOF){
            tokenizer->buffer[i] = pl701_EOF;
            if(feof(f)) return  PL701_OK;
            return  PL701_FAILED_READ_FILE;
        };

        tokenizer->buffer[i] = (uint8_t)ch;

    }

    return PL701_OK;
};

int 
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

    clearerr(f);
    for(int i = 0; i <  PL701_TOKENIZER_BLOCK_SZ; i++){
        int ch = getc(f);

        if(ch == EOF){
            *(bptr + i)  = pl701_EOF;
            if(feof(f)) return  PL701_OK;
            return  PL701_FAILED_READ_FILE;
        };

        *(bptr + i) = (uint8_t)ch;

    }

    return PL701_OK;
};

static int 
pl701_tokenizer_swap_buffer_back( Tokenizer * const tokenizer ) {

    uint8_t* bptr = tokenizer->buffer;
    FILE* f = tokenizer->source_file;

    tokenizer->foward_pos = bptr;
    tokenizer->current_buffer = 0;

    clearerr(f);
    for(int i = 0; i <  PL701_TOKENIZER_BLOCK_SZ; i++){
        int ch = getc(f);

        if(ch == EOF){
            *(bptr + i) = pl701_EOF;
            if(feof(f)) return  PL701_OK;
            return  PL701_FAILED_READ_FILE;
        };

        *(bptr + i)  = (uint8_t)ch;

    }

    return PL701_OK;

};

static int 
pl701_copy_token( Token** token, Tokenizer const * tokenizer,
                  const StatesMask_t mask){

     uint8_t* cpos = tokenizer->current_pos;
     uint8_t* fpos = tokenizer->foward_pos;

     char buffer[PL701_TOKENIZER_BLOCK_SZ];

     int count = 1;
     while(cpos != fpos){
        if(*cpos == pl701_EOB){
            if(tokenizer->current_buffer) {
                cpos = tokenizer->buffer_back;
            } else {
                cpos = tokenizer->buffer;
            }
        }

        buffer[count - 1] = *cpos;
        count++; cpos++;
     };

    buffer[count - 1] = *cpos;


    TokenTag tag = TK_UNDEFINED;
    pl701_get_tag_from_mask(mask, &tag);
    return pl701_new_token( token, buffer, count, tag);

    };


static int
pl701_new_token( Token ** token, char* name, size_t size, TokenTag tag){
     
     Token* tk;
     tk = (Token*)malloc(sizeof(Token));

     // TODO: should handle unicode in future.
     tk->name = (char*)malloc((size + 1)* sizeof(char));
     memcpy(tk->name, name, size * sizeof(char));
     *(tk->name + size) = '\0'; // Make the string null terminated.
     tk->name_len = size;
     tk->tag = tag;
     *token = tk;

     return PL701_OK; 

};

static int 
pl701_free_token( Token * token){

    if(!token) return PL701_MISUSE;

    free(token->name);
    free(token);
    return PL701_OK; 
};


int 
pl701_next_token(Tokenizer * const tokenizer, Token** token, int* sucess){
    
    StatesMask_t mask = Pl701_TK_MASK(TK_ST_INITIAL); // Initial state.

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

            }else if(mask & pl701_final_states) {
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
        ch = *(tokenizer->foward_pos);
    
        lpos++;
        if(ch == '\n') {lpos = 0; line++; };
    };
};


static StatesMask_t 
pl701_find_epsilon_closure(const StatesMask_t mask){
    StatesMask_t result_mask = 0;

    for(int i = 0; i < PL701_TK_STATE_COUNT; i++){
        //Looping over all the states in mask.
        if(mask & (1 << i)){
            StatesMask_t states = pl701_transtb_single_entry(i, PL701_TK_EPSILON);
            // Getting the states reached by one epsilon transition.

            StatesMask_t states_rest = pl701_find_epsilon_closure((~result_mask) & states);
            // Getting the rest of the states by recursion. Only searching the states that 
            // are not in result_mask.

            result_mask |= states;
            result_mask |= states_rest;
        };
    };
    return result_mask;
};


static StatesMask_t 
pl701_query_transition_table(const StatesMask_t mask,
                            const char ch){

    StatesMask_t result_mask = 0;

    for(int i = 0; i < PL701_TK_STATE_COUNT; i++){
        if(mask & (1 << i)){
            StatesMask_t states = pl701_transtb_single_entry(i, (int)ch);
            StatesMask_t closure = pl701_find_epsilon_closure(states);

            result_mask |= states;
            result_mask |= closure;
     };
    };

    return result_mask;

};

static StatesMask_t 
pl701_transtb_single_entry(TokenizerState state, int character){

    return pl701_transition_table__[state][character];
};



static void 
pl701_get_tag_from_mask(const StatesMask_t mask, TokenTag* const tag){


};


