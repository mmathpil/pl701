#include "tokenizer_tests.h"
#include "src/error.h"

int 
tokenizer_run_all_tests(int argc, char* argv[]){

    tokenizer_basic_tests();
    return 0;

}


static Tokenizer* 
init_and_load_tokenizer(const char* filename){

   Tokenizer* tkizer;
   if( pl701_init_tokenizer( &tkizer, filename ) != PL701_OK ) { 
        printf("Tokenizer initalize eror.\n");
        return NULL;
    };

    if( pl701_tokenizer_load_file(tkizer) != PL701_OK ){
        printf("Tokenizer failed to load file.\n"); 
        return NULL;
   };

 return tkizer;
};


static int 
wite_to_file(const char* filename, const char* content){
    
    FILE* f = fopen(filename, "w");
    if(!f) {
        printf("Failed to open file %s", filename);
        return PL701_FAILED_OPEN_FILE; // TODO: Properly raise error.
    };
    
    int size = 0;
    const char* cptr = content;
    while(*cptr++) size++;

    int res = fwrite(content, sizeof(char),size ,f);
    if(!res) {
        printf("Failed write to file %s", filename);
        return PL701_FAILED_WRITE_FILE;
    };

    fclose(f);

    return  PL701_OK;
};

static void 
tokenizer_basic_tests(){
    // Arrange
    static const char* filename = "tokenizer_basic_tests.txt";

    if (wite_to_file(filename, "3342dljfksdl4h3cabb") != PL701_OK) return;
    Tokenizer* tkinzer = init_and_load_tokenizer(filename);
    if(!tkinzer) return;

    Token* token;
    int res, success;

    // Act
    res = pl701_next_token(tkinzer, &token, &success);

    // Assert
    if(res != PL701_OK){
        printf("Tokenizer failed.\n");
        return; 
    };

    if(!success){
        printf("Failed to find token.\n");
        return ;
   };

   printf("Token name : %s", token->name );



};



