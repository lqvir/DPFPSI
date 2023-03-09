#pragma once
#include "param.h"
#include "dpf/dpf_client.h"
namespace PSI{
    namespace AidServer
    {
        class AidServer{
        public:
            AidServer():DPFClient(0){};
            void init(const std::vector<LabelMask>& hash_table_input);
            DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            DPF::DPFResponseList DPFShareFullEval(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            
        private:

            std::vector<LabelMask> hash_table;
            size_t server_set_size_;
            DPF::dpf_client DPFClient;
        };
    } // namespace AidServer
    

} // namespace PSI