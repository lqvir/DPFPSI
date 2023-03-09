#pragma once

#include "dpf_common.h"

namespace PSI
{
    namespace DPF
    {
        class dpf_client{
        public:
            dpf_client() = delete;
            dpf_client(uint8_t party_number);
            ~dpf_client() = default;
      
            uint8_t Eval(std::bitset<DPF_INPUT_BIT_SIZE> x_bitset,const DPFKey& key);
            DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& dpfkeylist,const std::vector<LabelMask>& hash_table);
        private:
            uint8_t party_number_;
        };
    } // namespace DPF
    
} // namespace PSI
