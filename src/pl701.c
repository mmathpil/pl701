#include "stdio.h"
#include "tokenizer.h"
#include "error.h"

int main(int argc, char* argv[]){

    if(argc != 2) { printf("The name of the source is needed. \n"); return 0; }

   Tokenizer* tkizer;
   if( pl701_init_tokenizer( &tkizer, argv[1] ) != PL701_OK ) { 
        printf("Tokenizer initalize eror.\n");
        return 1;
    };

   if( pl701_tokenizer_load_file(tkizer) != PL701_OK ){
        printf("Tokenizer failed to load file.\n"); 
        return 1;
   };

  Token* token;
  int res, success;
  res = pl701_next_token(tkizer, &token);

  if(res != PL701_OK){
    printf("Tokenizer failed.\n");
    return 1; };

  if(tkizer->tkinzr_state == TKZR_FAILED){
    printf("Failed to find token.\n");
    return 1;
   };

   printf("Token name : %s", token->name );



  return 0;
}
