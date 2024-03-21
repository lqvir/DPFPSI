#pragma once

#include <wmmintrin.h>
#include <cstdint>
namespace PSI{
    namespace DPF{
        const uint8_t AESKEY[16] = {0xeb,0x28,0xcc,0xa6,0x5c,0x64,0x48,0xa1,0xf9,0x98,0x02,0xe8,0x68,0x0c,0x70,0xfd};


        struct mAES
            {
                /* data */
                mAES(){
                    
                    setKey(_mm_set_epi64x(((uint64_t*)AESKEY)[1], ((uint64_t*)AESKEY)[0]));
                }
                __m128i mRoundKeysEnc[11];

                __m128i keyGenHelper(__m128i key, __m128i keyRcon)
                {
                    keyRcon = _mm_shuffle_epi32(keyRcon, _MM_SHUFFLE(3, 3, 3, 3));
                    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
                    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
                    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
                    return _mm_xor_si128(key, keyRcon);
                }
                void setKey(const __m128i& key) {
                    mRoundKeysEnc[0] = key;
                    mRoundKeysEnc[1] = keyGenHelper(mRoundKeysEnc[0], _mm_aeskeygenassist_si128(mRoundKeysEnc[0], 0x01));
                    mRoundKeysEnc[2] = keyGenHelper(mRoundKeysEnc[1], _mm_aeskeygenassist_si128(mRoundKeysEnc[1], 0x02));
                    mRoundKeysEnc[3] = keyGenHelper(mRoundKeysEnc[2], _mm_aeskeygenassist_si128(mRoundKeysEnc[2], 0x04));
                    mRoundKeysEnc[4] = keyGenHelper(mRoundKeysEnc[3], _mm_aeskeygenassist_si128(mRoundKeysEnc[3], 0x08));
                    mRoundKeysEnc[5] = keyGenHelper(mRoundKeysEnc[4], _mm_aeskeygenassist_si128(mRoundKeysEnc[4], 0x10));
                    mRoundKeysEnc[6] = keyGenHelper(mRoundKeysEnc[5], _mm_aeskeygenassist_si128(mRoundKeysEnc[5], 0x20));
                    mRoundKeysEnc[7] = keyGenHelper(mRoundKeysEnc[6], _mm_aeskeygenassist_si128(mRoundKeysEnc[6], 0x40));
                    mRoundKeysEnc[8] = keyGenHelper(mRoundKeysEnc[7], _mm_aeskeygenassist_si128(mRoundKeysEnc[7], 0x80));
                    mRoundKeysEnc[9] = keyGenHelper(mRoundKeysEnc[8], _mm_aeskeygenassist_si128(mRoundKeysEnc[8], 0x1B));
                    mRoundKeysEnc[10] = keyGenHelper(mRoundKeysEnc[9], _mm_aeskeygenassist_si128(mRoundKeysEnc[9], 0x36));
                }

                void encryptECB(const __m128i& plaintext, __m128i& ciphertext) const {
                    ciphertext = _mm_xor_si128(plaintext, mRoundKeysEnc[0]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[1]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[2]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[3]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[4]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[5]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[6]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[7]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[8]);
                    ciphertext = _mm_aesenc_si128(ciphertext, mRoundKeysEnc[9]);
                    ciphertext = _mm_aesenclast_si128(ciphertext, mRoundKeysEnc[10]);
                }

    };
    }
   

}