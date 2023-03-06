#pragma once

#include "dpf_common.h"

namespace PSI
{
    namespace DPF
    {
        class dpf_client{
        public:
            dpf_client() = default;
            ~dpf_client() = default;
      
            uint8_t Eval(std::bitset<DPF_INPUT_BIT_SIZE> x_bitset,uint8_t party_number,const DPFKey& key);

        };
    } // namespace DPF
    
} // namespace PSI
