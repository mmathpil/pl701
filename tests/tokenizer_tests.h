#include "src/tokenizer.h"

int tokenizer_run_all_tests(int argc, char* argv[]);

// Utilities.
static Tokenizer* init_and_load_tokenizer(const char* filename);
static int wite_to_file(const char* filename, const char* content);


// Tests

static void tokenizer_ID_tests();
