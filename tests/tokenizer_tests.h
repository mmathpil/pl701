#ifndef PL701_TOKENIZER_TEST_H
#define PL701_TOKENIZER_TEST_H

#include "src/tokenizer.h"

int tokenizer_run_all_tests(int argc, char* argv[]);

// Utilities.
static Tokenizer* init_and_load_tokenizer(const char* filename);

// Write tokens to a specific file, seperated by newline.
static int write_tokens_to_file(const char* filename, const char* tokens[], int num);

// Retrun 1 if the content of both cmp and fixed are the same, otherwise return 0.
static int compare_tokens(Token* const cmp, Token* const fixed);



struct TokenizerTestInstance {
	Tokenizer* tkinzr;
	const char* filename;
	const char** token_list;
	uint32_t token_num;
	TokenTag tested_tag;

} typedef TokenizerTestInstance;


static int token_parse_test_tempate(TokenizerTestInstance* test);

// Tests

static void tokenizer_ID_tests();
static void tokenizer_floats_tests();
static void tokenizer_integer_tests();



#endif