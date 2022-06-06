#include "src/tokenizer.h"

int tokenizer_run_all_tests(int argc, char* argv[]);

// Utilities.
static Tokenizer* init_and_load_tokenizer(const char* filename);

// Write tokens to a specific file, seperated by newline.
static int write_tokens_to_file(const char* filename, const char* tokens[], int num);


// Tests

static void tokenizer_ID_tests();
