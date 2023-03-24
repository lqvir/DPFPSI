#pragma once


#include "oprf/dhoprf_sender.h"
#include "dpf/dpf_client.h"
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
            PSIServer(size_t server_set_size):server_set_size_(server_set_size),DPFClient(0){
                
            };
            std::vector<LabelMask> init(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            std::vector<std::string> process_query(const std::vector<std::string>& input);
            std::vector<std::string> process_query_thread(const std::vector<std::string>& input);
            DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist);
        private:
            std::vector<LabelMask> hash_table;

            size_t server_set_size_;
            OPRF::OPRFSender DHOPRFSender;
            DPF::dpf_client DPFClient;
        };


    } // namespace Server
    
} // namespace PSI
