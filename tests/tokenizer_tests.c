#include "tokenizer_tests.h"
#include "src/error.h"

int 
tokenizer_run_all_tests(int argc, char* argv[]){

    tokenizer_ID_tests();
    return 0;

}


static Tokenizer* 
init_and_load_tokenizer(const char* filename){

   Tokenizer* tkizer;

   int errcode = pl701_init_tokenizer( &tkizer, filename );    
   PL701_ASSERTC(errcode == PL701_OK, errcode, "Tokenizer initalize eror.")

   errcode =  pl701_tokenizer_load_file(tkizer);

   PL701_ASSERTC( errcode == PL701_OK ,errcode,  "Tokenizer failed to load file.")
   return tkizer;

};


static int 
write_tokens_to_file(const char* filename, const char* tokens[], int num){
    
    FILE* f = fopen(filename, "w");

    PL701_ASSERTC(f, PL701_FAILED_OPEN_FILE, "Failed to open file %s", filename)
    
    int size = 0;
    for (int i = 0; i < num; i++) {

        size = 0;
        const char* cptr = tokens[i];
        while (*cptr++) size++;
        int res = fwrite(tokens[i], sizeof(char), size, f);
        PL701_ASSERTC(res, PL701_FAILED_WRITE_FILE, "Failed write to file %s", filename)
        fwrite("\n", sizeof(char), 1, f);
    }

    fclose(f);

    return  PL701_OK;
};

static void 
tokenizer_ID_tests(){
    // Arrange
   
    static const char* filename = "tokenizer_id_tests.txt";
    const char* tokens[] = {
        "abc",
        "abcEFG",
        "a1b2c3",
        "a1-b2-c3",
        "__abc__",
        "_",
        "a",
        "_a0-b2SA_C--4"
    };

    int tokenum = sizeof(tokens) / sizeof(tokens[0]);

    if (write_tokens_to_file(filename, tokens, tokenum) != PL701_OK) return;

    Tokenizer* tkinzer = init_and_load_tokenizer(filename);
    if(!tkinzer) return;


    Token* token;
    int res;

    for (int count = 0; count < tokenum; count++) {


        // Act
        res = pl701_next_token(tkinzer, &token);

        // Assert

        if (res != PL701_OK) {
            PL701_ERRORC(res, "Tokenizer failed.")
                return;
        };

        if (tkinzer->tkinzr_state != TKZR_READY) {
            PL701_ERROR("Failed to find token.")
                return;
        };

        if (token->tag != TK_ID) {
            PL701_ERROR("Incorrect Token Tags.")
        }

        PL701_INFO("Token name : %s", token->name);
    }

};



