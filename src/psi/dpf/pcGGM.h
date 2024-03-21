#pragma once
#include "psi/param.h"
#include "dpf_common.h"
#include <droidCrypto/AES.h>
#include <droidCrypto/BitVector.h>
#include <droidCrypto/PRNG.h>
#include <droidCrypto/SecureRandom.h>
namespace PSI
{   
    namespace DPF
    {


        namespace pcGGM{

            namespace dr = droidCrypto;

            struct DPFKey{
                dr::block share;
                std::array<dr::block,DPF_EAYLY_HIGH - 1> cw;
                dr::block cw_n_HCW;
                uint8_t cw_n_last;
                std::array<uint8_t,DPF_COMPRESS_NODES_BYTE_NUMBER> cw_n1;
            };
            const __m128i LSB0Block =  _mm_set_epi64x(UINT64_MAX,UINT64_MAX-1);
            const __m128i LSB1Block =  _mm_set_epi64x(0,1);
            typedef  std::array<std::array<DPFKey,cuckoo::max_set_size>,cuckoo::block_num> DPFKeyList; 

            void GenKey(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1);
            void GenKey(size_t alpha,uint8_t beta,DPFKey& key0,DPFKey& key1);
            void GenTree(const DPFKey& key, uint64_t* ans);
            pcGGMLeafList GenTree(const DPFKey& key);
            // static void GetLeaf();
            pcGGMLeafList GenTree(const DPFKeyEarlyTerminal_ByArray key);

            void GenTreeRecursion(const DPFKey& key, uint64_t* ans);
        }

        


    } // namespace DPF
} // namespace PSI
