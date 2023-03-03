#pragma once


#include "oprf/dhoprf_sender.h"
#include "common/item.h"
#include "param.h"
#include <span>
#include <vector>
namespace PSI
{
    namespace Server
    {
        class PSIServer{
        public:
            PSIServer() = default;
            PSIServer(size_t server_set_size):server_set_size_(server_set_size){};
            std::vector<std::string> init(const std::vector<Item>& input,const std::vector<Label>& input_Label);
            std::vector<std::string> process_query(const std::vector<std::string>& input);
        private:
            size_t server_set_size_;
            OPRF::OPRFSender DHOPRFSender;
        };


    } // namespace Server
    
} // namespace PSI
