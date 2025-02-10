#pragma once
#include <openssl/evp.h>

extern EVP_MD_CTX *context;
extern EVP_MD *sha1;
extern char peer_id[21];
