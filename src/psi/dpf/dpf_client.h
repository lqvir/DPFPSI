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
            
            DPFResponseList DPFShare(const DPFKeyList& dpfkeylist,const std::vector<LabelMask>& hash_table);

            std::shared_ptr<DPFResponseList> FullEval(const DPFKeyList& dpfkeylist,const std::vector<LabelMask>& hash_table);
            std::shared_ptr<DPFResponseList> FullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> dpfkeylist,const std::vector<LabelMask>& hash_table);
            std::shared_ptr<DPFResponseList> FullEval(const std::shared_ptr<DPFKeyEarlyTerminal_ByArrayList> dpfkeylist,const std::vector<LabelMask>& hash_table);
            pcGGMLeafList DPFGenTree(const DPFKeyEarlyTerminal& key);
            pcGGMLeafList DPFGenTree(const DPFKey& key);
            
        private:
            uint8_t party_number_;
        };
    } // namespace DPF
    
} // namespace PSI
