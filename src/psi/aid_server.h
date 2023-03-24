#pragma once
#include "param.h"
#include "dpf/dpf_client.h"
namespace PSI{
    namespace AidServer
    {
        class AidServer{
        public:
            AidServer():DPFClient(0){};
            DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(
                const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist,
                const std::vector<LabelMask>& hash_table_input
                );
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(
                const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist,
                const std::vector<LabelMask>& hash_table_input
                );

            
        private:

            size_t server_set_size_;
            DPF::dpf_client DPFClient;
        };
    } // namespace AidServer
    

} // namespace PSI