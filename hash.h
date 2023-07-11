#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include "sha.h"

#define HASH_LEN SHA_DIGESTSIZE
#define READABLE_HASH_LEN (HASH_LEN * 2 + 1)
#ifndef INFO_LEN
#define INFO_LEN 1
#endif
#define HASH_INFO (HASH_LEN + INFO_LEN)

void hash(char *input, int len, char *digest);
void print_readable_digest(unsigned char const * const key, char *dest);

#endif

