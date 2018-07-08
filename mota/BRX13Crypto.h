//
// Created by Tim Uy on 4/14/18.
//

#ifndef BRX13CRYPTO_H
#define BRX13CRYPTO_H

#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>

#include "../crypto/sph_blake.h"
#include "../crypto/sph_bmw.h"
#include "../crypto/sph_groestl.h"
#include "../crypto/sph_jh.h"
#include "../crypto/sph_keccak.h"
#include "../crypto/sph_skein.h"
#include "../crypto/sph_luffa.h"
#include "../crypto/sph_cubehash.h"
#include "../crypto/sph_shavite.h"
#include "../crypto/sph_simd.h"
#include "../crypto/sph_echo.h"
#include "../crypto/sph_hamsi.h"
#include "../crypto/sph_fugue.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef GLOBALDEFINED
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL sph_blake512_context     z_blake;
GLOBAL sph_bmw512_context       z_bmw;
GLOBAL sph_groestl512_context   z_groestl;
GLOBAL sph_jh512_context        z_jh;
GLOBAL sph_keccak512_context    z_keccak;
GLOBAL sph_skein512_context     z_skein;
GLOBAL sph_luffa512_context     z_luffa;
GLOBAL sph_cubehash512_context  z_cubehash;
GLOBAL sph_shavite512_context   z_shavite;
GLOBAL sph_simd512_context      z_simd;
GLOBAL sph_echo512_context      z_echo;
GLOBAL sph_hamsi512_context      z_hamsi;
GLOBAL sph_fugue512_context      z_fugue;

#define fillz() do { \
    sph_blake512_init(&z_blake); \
    sph_bmw512_init(&z_bmw); \
    sph_groestl512_init(&z_groestl); \
    sph_jh512_init(&z_jh); \
    sph_keccak512_init(&z_keccak); \
    sph_skein512_init(&z_skein); \
    sph_luffa512_init(&z_luffa); \
    sph_cubehash512_init(&z_cubehash); \
    sph_shavite512_init(&z_shavite); \
    sph_simd512_init(&z_simd); \
    sph_echo512_init(&z_echo); \
    sph_hamsi512_init(&z_hamsi); \
    sph_fugue512_init(&z_fugue); \
} while (0)

#define ZBLAKE (memcpy(&ctx_blake, &z_blake, sizeof(z_blake)))
#define ZBMW (memcpy(&ctx_bmw, &z_bmw, sizeof(z_bmw)))
#define ZGROESTL (memcpy(&ctx_groestl, &z_groestl, sizeof(z_groestl)))
#define ZJH (memcpy(&ctx_jh, &z_jh, sizeof(z_jh)))
#define ZKECCAK (memcpy(&ctx_keccak, &z_keccak, sizeof(z_keccak)))
#define ZSKEIN (memcpy(&ctx_skein, &z_skein, sizeof(z_skein)))
#define ZHAMSI (memcpy(&ctx_hamsi, &z_hamsi, sizeof(z_hamsi)))
#define ZFUGUE (memcpy(&ctx_fugue, &z_fugue, sizeof(z_fugue)))

void BRX13(void *md32, const void *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif //BREADWALLET_ANDROID_BRX13CRYPTO_H
