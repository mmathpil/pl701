#include "stdlib.h"
#include "string.h"
#include "tokenizer.h"
#include "ctype.h"

int 
pl701_init_tokenizer( Tokenizer ** tokenizer, 
                                const char* src_file){

#ifdef _WIN32
    FILE* f = fopen(src_file, "rt");
#else
    FILE* f = fopen(src_file, "r");
#endif

    if(!f) return PL701_FAILED_OPEN_FILE;

    Tokenizer* tkzr;

    tkzr = (Tokenizer*)malloc(sizeof(Tokenizer));
    tkzr->source_file = f;

    tkzr->current_pos = tkzr->buffer;
    tkzr->foward_pos =  tkzr->buffer;

    tkzr->line_count = 1;
    tkzr->line_pos = 0;

    tkzr->buffer[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;
    tkzr->buffer_back[PL701_TOKENIZER_BLOCK_SZ] = pl701_EOB;

    tkzr->current_buffer = 0;
    tkzr->parsestate_ptr = &(tkzr->parsestate_bf);

    tkzr->tkinzr_state = TKZR_READY;
    tkzr->load_on_swap = 1;

    *tokenizer = tkzr;
    return pl701_init_transition_table();

};

static int 
pl701_init_transition_table(){

    for(int i = 0; i < PL701_TK_STATE_COUNT; i++){
        for(int j = 0; j < PL701_TK_CARACTER_COUNT; j++){
             pl701_transition_table__[i][j] = 0;
        }; };

    // Entry point of the transition table.
    Pl701_TK_TBEPSILON(TK_ST_INITIAL, Pl701_TK_MASK(TK_ST1) | Pl701_TK_MASK(TK_ST2));
    
    Pl701_TK_TBSETS(TK_ST1, "\t\n\v\f\r ", Pl701_TK_MASK(TK_ST2))

    Pl701_TK_TBEPSILON(TK_ST2,  Pl701_TK_MASK(TK_ST1)| Pl701_TK_MASK(TK_ST3));

    // Parsing of the ID.
    Pl701_TK_TBRANGE(TK_ST3, 'a','z' , Pl701_TK_MASK(TK_ST4));
    Pl701_TK_TBRANGE(TK_ST3, 'A', 'Z', Pl701_TK_MASK(TK_ST4));
    Pl701_TK_TBSETS(TK_ST3, "_", Pl701_TK_MASK(TK_ST4));

    Pl701_TK_TBRANGE(TK_ST4, 'a', 'z', Pl701_TK_MASK(TK_ST5));
    Pl701_TK_TBRANGE(TK_ST4, 'A', 'Z', Pl701_TK_MASK(TK_ST5));
    Pl701_TK_TBRANGE(TK_ST4, '0', '9', Pl701_TK_MASK(TK_ST5));
    Pl701_TK_TBSETS(TK_ST4, "_-'", Pl701_TK_MASK(TK_ST5));
    Pl701_TK_TBEPSILON(TK_ST4, Pl701_TK_MASK(TK_ST6));

    Pl701_TK_TBEPSILON(TK_ST5, Pl701_TK_MASK(TK_ST4)|Pl701_TK_MASK(TK_ST6));
    // TK_ST6 is the terminating state of ID.



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

    if (tokenizer->load_on_swap) {
        clearerr(f);
        for (int i = 0; i < PL701_TOKENIZER_BLOCK_SZ; i++) {
            int ch = getc(f);

            if (ch == EOF) {
                *(bptr + i) = pl701_EOF;
                if (feof(f)) return  PL701_OK;
                return  PL701_FAILED_READ_FILE;
            };

            *(bptr + i) = (uint8_t)ch;

        }
    };

    return PL701_OK;
};

static int 
pl701_tokenizer_swap_buffer_back( Tokenizer * const tokenizer ) {

    uint8_t* bptr = tokenizer->buffer;
    FILE* f = tokenizer->source_file;

    tokenizer->foward_pos = bptr;
    tokenizer->current_buffer = 0;

    if (tokenizer->load_on_swap) {
        clearerr(f);
        for (int i = 0; i < PL701_TOKENIZER_BLOCK_SZ; i++) {
            int ch = getc(f);

            if (ch == EOF) {
                *(bptr + i) = pl701_EOF;
                if (feof(f)) return  PL701_OK;
                return  PL701_FAILED_READ_FILE;
            };

            *(bptr + i) = (uint8_t)ch;

        }
    };

    return PL701_OK;

};


static int
pl701_rewind_char(Tokenizer* const tokenizer) {

    char* cp = tokenizer->foward_pos;
    
    if (cp == tokenizer->current_pos) {
        PL701_WARN("Cannot continue to rewind the tokenizer.");
        return  PL701_OK;
    };

    if (cp == tokenizer->buffer) {
        tokenizer->current_buffer = 1;
        tokenizer->load_on_swap = 0;
        tokenizer->foward_pos = (tokenizer->buffer_back + PL701_TOKENIZER_BLOCK_SZ - 1);

    }
    else if (cp == tokenizer->buffer_back) {
        tokenizer->current_buffer = 0;
        tokenizer->load_on_swap = 0;
        tokenizer->foward_pos = (tokenizer->buffer + +PL701_TOKENIZER_BLOCK_SZ - 1);
    }
    else {
        tokenizer->foward_pos--;
    };

    return PL701_OK;
};


static int 
pl701_next_char(Tokenizer* const tokenizer, char* ch) {
    
    char c = *(tokenizer->foward_pos);

    if (c == pl701_EOF) { *ch = c; return PL701_OK; };

    if (c == pl701_EOB) {
        if (tokenizer->current_buffer) {
            pl701_tokenizer_swap_buffer_back(tokenizer);
        }
        else {
            pl701_tokenizer_swap_buffer(tokenizer);
        }
       
        // After we swap the buffer, we want the the buffer continues to load after that.
        tokenizer->load_on_swap = 1;

    }

    *ch = *(tokenizer->foward_pos);

    tokenizer->foward_pos++;
    return PL701_OK;

};

static int 
pl701_copy_token( Token** token, Tokenizer const * tokenizer){

     uint8_t* cpos = tokenizer->current_pos;
     uint8_t* fpos = tokenizer->foward_pos; 

     char buffer[PL701_TOKENIZER_BLOCK_SZ];

     int count = 1;
     while(cpos != fpos ){
        if(*cpos == pl701_EOB){
            if(tokenizer->current_buffer) {
                cpos = tokenizer->buffer_back;
            } else {
                cpos = tokenizer->buffer;
            }

            if (cpos == fpos) break; 
        }

        if (!isspace(*cpos)) {
            buffer[count - 1] = *cpos;
            count++;
        };

         cpos++;
     };

     if (!isspace(*cpos)) buffer[count - 1] = *cpos;
   
    TokenTag tag = TK_UNDEFINED;
    pl701_get_tag_from_mask(*(tokenizer->parsestate_ptr), &tag);
    return pl701_new_token( token, buffer, count - 1, tag);

    };

static int 
pl701_rewind_tokenizer(Tokenizer* const tokenizer) {

    
    StatesMask_t* base = tokenizer->parsestate_bf;
    StatesMask_t* ptr = tokenizer->parsestate_ptr;

    do {
        ptr--;
        if ((*ptr) & pl701_final_states) {
            tokenizer->tkinzr_state = TKZR_SUCCESS;
            tokenizer->parsestate_ptr = ptr;
            return PL701_OK;
        }
        pl701_rewind_char(tokenizer);
    } while ( (base + 1) != ptr);
    // base points to the state calculated from epsilon closure, so we don't need to rewind that far.

    tokenizer->tkinzr_state = TKZR_FAILED;
    tokenizer->parsestate_ptr = ptr;
    return PL701_OK;
}


static int
pl701_update_tokenizer(Tokenizer* const tokenizer) {

    uint8_t* cpos = tokenizer->current_pos;
    uint8_t nextcpos;

    uint8_t* fpos = tokenizer->foward_pos;

    uint32_t line = tokenizer->line_count;
    uint32_t pos = tokenizer->line_pos;

    while ( cpos != fpos ) {

        switch (*cpos) {
        case  PL701_TOKENIZER_EOB:
            // Note that the current_buffer indicates the positions of foward pointer. 
            if (tokenizer->current_buffer) {
                cpos = tokenizer->buffer_back;
            } else {
                cpos = tokenizer->buffer;
            };
            break;
        case PL701_TOKENIZER_EOF:
            PL701_WARN("EOF reached before finding the foward pointer.");
            goto WHILE_END;
        case '\n':
            line++; pos = 0; cpos++;
            break;
        default:
            pos++; cpos++;
        }

    };

WHILE_END:

    // Updating the positions.
    tokenizer->line_count = line;
    tokenizer->line_pos = pos;
    tokenizer->current_pos = tokenizer->foward_pos;

    // Clearly parse states buffer.
    tokenizer->parsestate_ptr = tokenizer->parsestate_bf;

    // Updating tokenizer states.
    if (tokenizer->foward_pos == pl701_EOF) {
        tokenizer->tkinzr_state = TKZR_EOF;
    }
    else {
        tokenizer->tkinzr_state = TKZR_READY;
    }

    return PL701_OK;
}

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

int 
pl701_free_token( Token * token){

    if(!token) return PL701_MISUSE;

    free(token->name);
    free(token);
    return PL701_OK; 
};


int 
pl701_next_token(Tokenizer * const tokenizer, Token** token){
    
    StatesMask_t mask = Pl701_TK_MASK(TK_ST_INITIAL); // Initial state.
    StatesMask_t final_states = 0;
    
    mask = pl701_find_epsilon_closure(mask);
    *(tokenizer->parsestate_ptr) = mask;
     (tokenizer->parsestate_ptr)++;

     char ch;

     for (int count = 1; count < PL701_TOKENIZER_BLOCK_SZ; count ++) {

        pl701_next_char(tokenizer, &ch);

        mask = pl701_query_transition_table(mask, ch);

        *(tokenizer->parsestate_ptr) = mask;
        (tokenizer->parsestate_ptr)++;

        if ((ch == PL701_TOKENIZER_EOF) || !mask) {
            // When end of the file reached or the tokenizer reached an end state,
            // start rewinding and find a vaild token.

            pl701_rewind_tokenizer(tokenizer);
            if (tokenizer->tkinzr_state == TKZR_SUCCESS) {
                pl701_copy_token(token, tokenizer);
                return pl701_update_tokenizer(tokenizer);
            }
            else if(tokenizer->tkinzr_state == TKZR_FAILED) {
                return PL701_OK;
            }
            else {
                PL701_CRITICAL("Tokenizer internal error!");
            };
            
        }



    };

     PL701_WARN("The maximum token size reached. Start rewinding anyway.")

     pl701_rewind_tokenizer(tokenizer);
     if (tokenizer->tkinzr_state == TKZR_SUCCESS) {
         pl701_copy_token(token, tokenizer);
     };
     return pl701_update_tokenizer(tokenizer);
}




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
pl701_transtb_single_entry(ParseState state, int character){

    return pl701_transition_table__[state][character];
};



static void 
pl701_get_tag_from_mask(const StatesMask_t mask, TokenTag* const tag){

    if (mask & Pl701_TK_MASK(TK_ST6)) *tag = TK_ID;

};


