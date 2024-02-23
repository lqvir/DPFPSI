#pragma once
#include "pcGGM.h"

#include "dpf_common.h"
namespace PSI{
    namespace DPF{
        namespace DPFServer{


            void Gen(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1);

            void DPFConstruct(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA);

            inline uint8_t reconstruct(uint8_t share1,uint8_t share2){
                return (share1&1)+(share2&1);
            }

        }
     
 
       
        
        

    }

} // namespace PSI