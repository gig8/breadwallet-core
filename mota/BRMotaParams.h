//
//  BRMotaParams.h
//
//  Created by Aaron Voisine on 1/10/18.
//  Adapted for Motacoin on 4/3/18.
//  Copyright (c) 2019 breadwallet LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BRMotaParams_h
#define BRMotaParams_h

#include "BRChainParams.h"
#include "BRPeer.h"
#include "BRInt.h"

static const char *BRMotaDNSSeeds[] = {
    "explorer.motacoin.vip", NULL
};

static const char *BRMotaTestNetDNSSeeds[] = {
    "explorer.motacoin.vip", NULL
};
/*
    algorithm: X13
    merkle hash: b3d8b0c93a5a36fc81b2cdc8a5ca2adb781f4d22530cd4671cf2e2c8181754d6
    pszTimestamp: POTUS Tweets:'And yet, there is NO COLLUSION!'
    pubkey:
    time: 1521404888
    bits: 0x1e0fffff
    Searching for genesis hash..
    genesis hash found!
    nonce: 158782
    genesis hash: 00000db79f5fe2062d31f62b76bd37b1d31373614a8ef2a85f60bc30eb411500
*/
static const BRCheckPoint BRMotaTestNetCheckpoints[] = {
    {       0, uint256("00000fea25f87416682baa54946a1d156909d0959588eb573a0ae16a64230c61"), 1521404888, 0x1e0fffff }
};

// blockchain checkpoints - these are also used as starting points for partial chain downloads, so they must be at
// difficulty transition boundaries in order to verify the block difficulty at the immediately following transition
/*
    algorithm: X13
    merkle hash: b3d8b0c93a5a36fc81b2cdc8a5ca2adb781f4d22530cd4671cf2e2c8181754d6
    pszTimestamp: POTUS Tweets:'And yet, there is NO COLLUSION!'
    pubkey:
    time: 1521404888
    bits: 0x1e0fffff
    Searching for genesis hash..
    genesis hash found!
    nonce: 145590
    genesis hash: 00000fea25f87416682baa54946a1d156909d0959588eb573a0ae16a64230c61
*/
static const BRCheckPoint BRMotaCheckpoints[] = {
    {       0, uint256("00000fea25f87416682baa54946a1d156909d0959588eb573a0ae16a64230c61"), 1521404888, 0x1e0fffff },
    {    9327, uint256("00000000003e457413b915a18a07f046b458f3d03bd884b37fc467f0aaaa2ba6"), 1523748132, 0x1b41dc07 }
};

static int BRMotaVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet, long maxProofOfWork, long maxProofOfStake)
{
    const BRMerkleBlock *previous, *b = NULL;
    uint32_t i;

    assert(block != NULL);
    assert(blockSet != NULL);


    // forget about all this
    return 1;

    // check if we hit a difficulty transition, and find previous transition block
    if ((block->height % BLOCK_DIFFICULTY_INTERVAL) == 0) {
        for (i = 0, b = block; b && i < BLOCK_DIFFICULTY_INTERVAL; i++) {
            b = BRSetGet(blockSet, &b->prevBlock);
        }
    }

    previous = BRSetGet(blockSet, &block->prevBlock);
    return BRMerkleBlockVerifyDifficulty(block, previous, (b) ? b->timestamp : 0, maxProofOfWork, maxProofOfStake);
}

static int BRMotaTestNetVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet, long maxProofOfWork, long maxProofOfStake)
{
    return 1; // XXX skip testnet difficulty check for now
}

static const BRChainParams BRMotaParams = {
    BRMotaDNSSeeds,
    17420,                // standardPort
    0x4a304a30,          // magicNumber
    0, // no services
    BRMotaVerifyDifficulty,
    BRMotaCheckpoints,
    sizeof(BRMotaCheckpoints)/sizeof(*BRMotaCheckpoints),
    0x20, // forkId
    ALGO_X13,   // algoId
    70013,
    60013,
    0x1e0fffff, // bnProofOfWorkLimit(~uint256(0) >> 20);
    0x1e0fffff, // may fail when we go to POS - bnProofOfStakeLimit(~uint256(0) >> 24);
    80, // blockHeaderSize
    2,  // blockHeaderSpacing
    1500, // blockHeaderNLimit
};

static const BRChainParams BRMotaTestNetParams = {
    BRMotaTestNetDNSSeeds,
    26000,               // standardPort
    0x4b304a30,          // magicNumber
    0, // no services
    BRMotaTestNetVerifyDifficulty,
    BRMotaTestNetCheckpoints,
    sizeof(BRMotaTestNetCheckpoints)/sizeof(*BRMotaTestNetCheckpoints),
    0x20, // forkId
    ALGO_X13,   // algoId
    70013,
    60013,
    0x1e0fffff, // bnProofOfWorkLimitTestNet(~uint256(0) >> 16);
    0x1e0fffff, // may fail when we go to POS - bnProofOfStakeLimitTestNet(~uint256(0) >> 30);
    80, // blockHeaderSize
    2,  // blockHeaderSpacing
    1500, // blockHeaderNLimit
};

#endif // BRMotaParams_h
