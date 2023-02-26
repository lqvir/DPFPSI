#pragma once

#include "dpf_common.h"
namespace PSI{
    namespace DPF{
        class dpf_server
        {
        public:
            dpf_server() = default;
            ~dpf_server() = default;

            void init();
            void Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta = 1);
            DPFKey get_key0();
            DPFKey get_key1();
            uint8_t reconstruct(uint8_t share1,uint8_t share2){
                return (share1&1)+(share2&1);
            }
        private:
            /* data */

            DPFKey key0,key1;
            std::bitset<Lambda> Delta;
        };
        
        

    }

} // namespace PSI