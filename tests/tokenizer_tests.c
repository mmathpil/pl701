#include "string.h"
#include "tokenizer_tests.h"
#include "src/error.h"


enum TokenizerTestResult {
    TEST_FAILED = 0,
    TEST_SUCCESS = 1
};


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


static int
compare_tokens(Token* const cmp, Token* const fixed) {
    int res = 1;
    res = res && (strcmp(cmp->name, fixed->name) == 0);
    res = res && (cmp->name_len == fixed->name_len);
    res = res && (cmp->tag == fixed->tag);

    return res;
};


static int 
token_parse_test_tempate(TokenizerTestInstance* test) {

    if (write_tokens_to_file(test->filename, test->token_list, test->token_num) != PL701_OK) return;

    Tokenizer* tkinzer = init_and_load_tokenizer(test->filename);
    if (!tkinzer) return TEST_FAILED;


    Token* tested_token, fixed_token;
    int res;
    int linecount = 1;

    for (int count = 0; count < test->token_num; count++) {


        // Act
        res = pl701_next_token(tkinzer, &tested_token);

        // Assert

        if (res != PL701_OK) {
            PL701_ERRORC(res, "Tokenizer failed.")
                return TEST_FAILED;
        };

        if (tkinzer->tkinzr_state != TKZR_READY) {
            PL701_ERROR("Failed to find token.")
                return TEST_FAILED;
        };

        fixed_token.name = test->token_list[count];
        fixed_token.name_len = strlen(test->token_list[count]);
        fixed_token.tag = TK_ID;

        if (!compare_tokens(tested_token, &fixed_token)) {
            PL701_ERROR("Token content incorrect : %s", tested_token->name);
            return TEST_FAILED;
        }

        //Test the positions of the tokenizer.
        if (tkinzer->line_count != linecount) {
            PL701_ERROR("Incorrect line count: %d", tkinzer->line_count);
            return TEST_FAILED;
        }
        if (tkinzer->line_pos != strlen(test->token_list[count])) {
            PL701_ERROR("Incorrect line position: %d", tkinzer->line_pos);
            return TEST_FAILED;
        }

        linecount++;
        pl701_free_token(tested_token);
    };

    pl701_tokenizer_finalized(tkinzer);

    return TEST_SUCCESS;
};


static void 
tokenizer_ID_tests(){
    // Arrange
   


    static const char* fname = "tokenizer_id_tests.txt";
    const char* tokens[] = {
        "bc",
        "abcEFG",
        "a1b2c3",
        "a1-b2-c3",
        "__abc__",
        "_",
        "a",
        "_a0-b2SA_C--4"
    };

    TokenizerTestInstance test = {
        .tkinzr = NULL,
        .filename = fname,
        .token_list = tokens,
        .token_num = sizeof(tokens) / sizeof(tokens[0])
    };
    
    // Act

    if (token_parse_test_tempate(&test) == TEST_SUCCESS) {

        PL701_INFO("tokenizer_ID_tests finished!");
    }
    else {
        PL701_INFO("tokenizer_ID_tests failed!");
    };


};



