#pragma once
#include "param.h"
#include "dpf/dpf_client.h"

#include "common/Network/Session.h"
#include "common/Network/IOService.h"
#include "common/Network/Channel.h"
namespace PSI{
    namespace AidServer
    {
        class AidServer{
        public:
            AidServer(){};
            // DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(
            //     const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist,
            //     const std::vector<LabelMask>& hash_table_input
            //     );
            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(
                const std::unique_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList>& keylist,
                const std::vector<LabelMask>& hash_table_input
                );

            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(
                const std::unique_ptr<DPF::pcGGM::DPFKeyList>& keylist,
                const std::vector<LabelMask>& hash_table_input
                );
            void run(const std::vector<LabelMask>& input,std::vector<Channel>& chlsC);
            void runSIMD(const std::vector<LabelMask>& input,std::vector<Channel>& chlsC);

            void DHBasedPSI_start(std::string SelfAddress,std::string ServerAddress);
            void DHBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string ServerAddress);
            void GCBasedPSI_start(std::string SelfAddress,std::string ServerAddress);
            void GCBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string ServerAddress);
            
        private:

            size_t server_set_size_;
        };
    } // namespace AidServer
    

} // namespace PSI