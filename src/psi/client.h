#pragma once

#include "oprf/dhoprf_receiver.h"
#include "param.h"

#include <vector>
#include <span>

namespace PSI{
    namespace Client
    {
        class PSIClient{
            public:
                PSIClient() = default;
                PSIClient(size_t client_set_size):client_set_size_(client_set_size){};
                std::vector<std::string> OPRFQuery(const std::vector<Item>& input);
                std::vector<std::string> OPRFResponse(const std::vector<std::string>& response);
                void Cuckoo_All_location(const std::vector<std::string>& oprf_input);
            private:
                OPRF::OPRFReceiver DHOPRFReceiver;
                size_t client_set_size_;
                
        };

    } // namespace Client
}