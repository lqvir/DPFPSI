#pragma once
#include "param.h"
#include "dpf/dpf_client.h"
namespace PSI{
    namespace AidServer
    {
        class AidServer{
        public:
            AidServer() = default;
            void init(const std::vector<std::string>& hash_table_input);
            DPF::DPFResponseList DPFShare(DPF::DPFKeyList dpfkeylist);
        private:

            std::vector<std::string> hash_table;
            size_t server_set_size_;
            DPF::dpf_client DPFClient;

        };
    } // namespace AidServer
    

} // namespace PSI