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
    nonce: 145590
    genesis hash: 00000fea25f87416682baa54946a1d156909d0959588eb573a0ae16a64230c61
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
    nonce: 158782
    genesis hash: 00000db79f5fe2062d31f62b76bd37b1d31373614a8ef2a85f60bc30eb411500
*/
static const BRCheckPoint BRMotaCheckpoints[] = {
    {      0, uint256("00000db79f5fe2062d31f62b76bd37b1d31373614a8ef2a85f60bc30eb411500"), 1521404888, 0x1e0fffff }
};

static const BRMerkleBlock *_medianBlock(const BRMerkleBlock *b0, const BRSet *blockSet)
{
    const BRMerkleBlock *b, *b1 = NULL, *b2 = NULL;

    b1 = (b0) ? BRSetGet(blockSet, &b0->prevBlock) : NULL;
    b2 = (b1) ? BRSetGet(blockSet, &b1->prevBlock) : NULL;
    if (b0 && b2 && b0->timestamp > b2->timestamp) b = b0, b0 = b2, b2 = b;
    if (b0 && b1 && b0->timestamp > b1->timestamp) b = b0, b0 = b1, b1 = b;
    if (b1 && b2 && b1->timestamp > b2->timestamp) b = b1, b1 = b2, b2 = b;
    return (b0 && b1 && b2) ? b1 : NULL;
}

static int BRMotaVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
    int size, i, r = 1;
    const BRMerkleBlock *b, *first, *last;
    uint64_t target = 0, work = 0;
    uint32_t time;

    assert(block != NULL);
    assert(blockSet != NULL);
    
    if (block->height >= 504032) { // D601 hard fork height: https://reviews.bitcoinabc.org/D601
        first = (block) ? BRSetGet(blockSet, &block->prevBlock) : NULL;
        first = _medianBlock(first, blockSet);
        
        for (i = 0, last = block; last && i < 144; i++) {
            last = BRSetGet(blockSet, &last->prevBlock);
        }

        last = _medianBlock(last, blockSet);
        time = (first && last) ? first->timestamp - last->timestamp : 0;
        if (time > 288*10*60) time = 288*10*60;
        if (time < 72*10*60) time = 72*10*60;

        for (b = first; b && b != last;) {
            b = BRSetGet(blockSet, &b->prevBlock);
            
            // work += 2^256/(target + 1)
            size = (b) ? b->target >> 24 : 0;
            target = (b) ? b->target & 0x007fffff : 0;
            work += (1ULL << (32 - size)*8) - (target + 1);
        }

        // work = work*10*60/time
        work = work*10*60/time;

        // target = (2^256/work) - 1
        size = 0;
        while (((work + 1) >> size*8) != 0) size++;
        target = (1ULL << (32 - size)*8) - (work + 1);
        target |= ((32 - size) << 24);
        
        if (target > 0x1d00ffff) target = 0x1d00ffff; // max proof-of-work
        if (first && last && block->target != target) r = 1;
    }
        
    return r;
}

static int BRMotaTestNetVerifyDifficulty(const BRMerkleBlock *block, const BRSet *blockSet)
{
    return 1; // XXX skip testnet difficulty check for now
}

static const BRChainParams BRMotaParams = {
    BRMotaDNSSeeds,
    17420,                // standardPort
    0x304a304a,          // magicNumber
    SERVICES_NODE_MOTA, // services
    BRMotaVerifyDifficulty,
    BRMotaCheckpoints,
    sizeof(BRMotaCheckpoints)/sizeof(*BRMotaCheckpoints),
};

static const BRChainParams BRMotaTestNetParams = {
    BRMotaTestNetDNSSeeds,
    26000,               // standardPort
    0x304a304b,          // magicNumber
    SERVICES_NODE_MOTA, // services
    BRMotaTestNetVerifyDifficulty,
    BRMotaTestNetCheckpoints,
    sizeof(BRMotaTestNetCheckpoints)/sizeof(*BRMotaTestNetCheckpoints)
};

#endif // BRMotaParams_h
