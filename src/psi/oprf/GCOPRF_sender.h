#pragma once

#include "psi/oprf/GCOPRF.h"

namespace PSI
{
    namespace GCOPRF
    {
        
        class OPRFSender{
        public:
            OPRFSender::OPRFSender(droidCrypto::ChannelWrapper& chan);

            void base(size_t num_elements);
            void OPRFSender::Online
                (std::unique_ptr<std::vector<droidCrypto::block>> elements);
        private:
            droidCrypto::ChannelWrapper &channel_;
            std::array<uint8_t, 16> lowmc_key_;
            droidCrypto::SIMDLowMCCircuitPhases circ_;
        };

    } // namespace GCOPRF
    
} // namespace PSI
