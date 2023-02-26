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
            void init(const DPFKey& key_in){
                key = key_in;
            }
            uint8_t Eval(std::bitset<DPF_INPUT_BIT_SIZE> x_bitset,uint8_t party_number);
        private:
            DPFKey key;

        };
    } // namespace DPF
    
} // namespace PSI
