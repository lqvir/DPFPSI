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
      
            
            std::shared_ptr<DPFResponseList> FullEval(const std::shared_ptr<DPFKeyEarlyTerminal_ByArrayList> dpfkeylist,const std::vector<LabelMask>& hash_table);
           
            
        private:
            uint8_t party_number_;
        };
    } // namespace DPF
    
} // namespace PSI
