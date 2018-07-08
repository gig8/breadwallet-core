//
// Created by Tim Uy on 4/14/18.
//

#include "BRX13Crypto.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// X13
void BRX13(void *md32, const void *data, size_t len)
{
    sph_blake512_context     ctx_blake;
    sph_bmw512_context       ctx_bmw;
    sph_groestl512_context   ctx_groestl;
    sph_jh512_context        ctx_jh;
    sph_keccak512_context    ctx_keccak;
    sph_skein512_context     ctx_skein;
    sph_luffa512_context     ctx_luffa;
    sph_cubehash512_context  ctx_cubehash;
    sph_shavite512_context   ctx_shavite;
    sph_simd512_context      ctx_simd;
    sph_echo512_context      ctx_echo;
    sph_hamsi512_context     ctx_hamsi;
    sph_fugue512_context     ctx_fugue;
    static unsigned char     pblank[1];

    uint8_t t[32];

    assert(md32 != NULL);
    assert(data != NULL || len == 0);

    uint8_t hash0[64];
    uint8_t hash1[64];
    uint8_t hash2[64];
    uint8_t hash3[64];
    uint8_t hash4[64];
    uint8_t hash5[64];
    uint8_t hash6[64];
    uint8_t hash7[64];
    uint8_t hash8[64];
    uint8_t hash9[64];
    uint8_t hash10[64];
    uint8_t hash11[64];
    uint8_t hash12[64];

    sph_blake512_init(&ctx_blake);
    sph_blake512 (&ctx_blake, data, len);
    sph_blake512_close(&ctx_blake, &hash0[0]);

    sph_bmw512_init(&ctx_bmw);
    sph_bmw512 (&ctx_bmw, &hash0[0], 64);
    sph_bmw512_close(&ctx_bmw, &hash1[0]);

    sph_groestl512_init(&ctx_groestl);
    sph_groestl512 (&ctx_groestl, &hash1[0], 64);
    sph_groestl512_close(&ctx_groestl, &hash2[0]);

    sph_skein512_init(&ctx_skein);
    sph_skein512 (&ctx_skein, &hash2[0], 64);
    sph_skein512_close(&ctx_skein, &hash3[0]);

    sph_jh512_init(&ctx_jh);
    sph_jh512 (&ctx_jh, &hash3[0], 64);
    sph_jh512_close(&ctx_jh, &hash4[0]);

    sph_keccak512_init(&ctx_keccak);
    sph_keccak512 (&ctx_keccak, &hash4[0], 64);
    sph_keccak512_close(&ctx_keccak, &hash5[0]);

    sph_luffa512_init(&ctx_luffa);
    sph_luffa512 (&ctx_luffa, &hash5[0], 64);
    sph_luffa512_close(&ctx_luffa, &hash6[0]);

    sph_cubehash512_init(&ctx_cubehash);
    sph_cubehash512 (&ctx_cubehash, &hash6[0], 64);
    sph_cubehash512_close(&ctx_cubehash, &hash7[0]);

    sph_shavite512_init(&ctx_shavite);
    sph_shavite512(&ctx_shavite, &hash7[0], 64);
    sph_shavite512_close(&ctx_shavite, &hash8[0]);

    sph_simd512_init(&ctx_simd);
    sph_simd512 (&ctx_simd, &hash8[0], 64);
    sph_simd512_close(&ctx_simd, &hash9[0]);

    sph_echo512_init(&ctx_echo);
    sph_echo512 (&ctx_echo, &hash9[0], 64);
    sph_echo512_close(&ctx_echo, &hash10[0]);

    sph_hamsi512_init(&ctx_hamsi);
    sph_hamsi512 (&ctx_hamsi, &hash10[0], 64);
    sph_hamsi512_close(&ctx_hamsi, &hash11[0]);

    sph_fugue512_init(&ctx_fugue);
    sph_fugue512 (&ctx_fugue, &hash11[0], 64);
    sph_fugue512_close(&ctx_fugue, &hash12[0]);

    memcpy(md32, hash12, 32);
}
