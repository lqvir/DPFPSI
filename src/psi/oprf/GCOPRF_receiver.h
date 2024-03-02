#pragma once
#include "psi/oprf/GCOPRF.h"
#include <droidCrypto/gc/circuits/LowMCCircuit.h>
#include <vector>

namespace PSI
{
    namespace GCOPRF
    {
        class OPRFReciver{
        public:
            OPRFReciver(droidCrypto::ChannelWrapper& chan);
            
            void base(size_t num_elements);
            std::unique_ptr<std::vector<GCOPRFValue>> Online
                (std::unique_ptr<std::vector<droidCrypto::block>> elements);
            size_t getComCost();
        private:
            droidCrypto::ChannelWrapper &channel_;
            std::array<uint8_t, 16> lowmc_key_;
            droidCrypto::SIMDLowMCCircuitPhases circ_;
        };
        
    } // namespace GCOPRF
    
} // namespace PSI
