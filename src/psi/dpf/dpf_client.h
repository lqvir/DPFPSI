#pragma once

#include "dpf_common.h"
#include "pcGGM.h"

namespace PSI
{
    namespace DPF
    {
        namespace DPFClient{
            std::unique_ptr<DPFResponseList> FullEval(const std::unique_ptr<DPFKeyEarlyTerminal_ByArrayList>& dpfkeylist,const std::vector<LabelMask>& hash_table, size_t party_number);
            std::unique_ptr<DPFResponseList> FullEval(const std::unique_ptr<pcGGM::DPFKeyList>& dpfkeylist,const std::vector<LabelMask>& hash_table,size_t party_number);
        }
       
    } // namespace DPF
    
} // namespace PSI
