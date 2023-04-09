#pragma once

#include "dpf_common.h"
namespace PSI{
    namespace DPF{
        class dpf_server
        {
        public:
            dpf_server() = default;
            ~dpf_server() = default;

            void Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta,DPFKey& key0,DPFKey& key1);
            void Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta,DPFKeyEarlyTerminal& key0,DPFKeyEarlyTerminal& key1);
            void Gen(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1);

            void DPFConstruct(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA);

            uint8_t reconstruct(uint8_t share1,uint8_t share2){
                return (share1&1)+(share2&1);
            }
        private:
            /* data */

        };
        
        

    }

} // namespace PSI