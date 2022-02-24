#include "stdio.h"
#include "tokenizer.h"


int main(int argc, char* argv[]){

    if(argc != 2) { printf("The name of the source is needed. \n"); return 0; }

   Tokenizer* tkizer;
   if( pl701_init_tokenizer( &tkizer, argv[1] ) != PL701_OK ) 
            printf("Tokenizer initalize eror.\n");


  Token* token;
  int res, success;
  res = pl701_next_token(tkizer, &token, &success);

  if(res != PL701_OK) printf("Tokenizer failed.\n");
  if(!success)  printf("Failed to find token.\n");



  return 0;
}