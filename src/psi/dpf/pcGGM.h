#pragma once
#include "psi/param.h"
#include "dpf_common.h"

namespace PSI
{   
    namespace DPF
    {
        namespace pcGGM{

            void GenKey(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1);


            // static void GetLeaf();
            pcGGMLeafList GenTree(const DPFKeyEarlyTerminal_ByArray key);
        }

        


    } // namespace DPF
} // namespace PSI
