#pragma once

#include "psi/oprf/GCOPRF.h"
#include <droidCrypto/gc/circuits/LowMCCircuit.h>

namespace PSI
{
    namespace GCOPRF
    {
        
        class OPRFSender{
        public:
        
            OPRFSender(droidCrypto::ChannelWrapper& chan);
            std::unique_ptr<std::vector<GCOPRFValue>>
                setup(std::unique_ptr<std::vector<droidCrypto::block>> elements);

            void base();
            void Online();

            
        private:
            droidCrypto::ChannelWrapper &channel_;
            std::array<uint8_t, 16> lowmc_key_;
            droidCrypto::SIMDLowMCCircuitPhases circ_;
        };

    } // namespace GCOPRF
    
} // namespace PSI
