#pragma once


#include "oprf/dhoprf_sender.h"
#include "dpf/dpf_client.h"
#include "common/item.h"
#include "param.h"
#include <span>
#include <vector>


#include "common/Network/Session.h"
#include "common/Network/IOService.h"
#include "common/Network/Channel.h"
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
            std::vector<LabelMask> init_FourQ(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            std::vector<OPRF::OPRFPoint> process_query(const std::vector<OPRF::OPRFPoint>& input);
            std::vector<OPRF::OPRFPoint> process_query_thread(const std::vector<OPRF::OPRFPoint>& input);
            std::vector<OPRF::OPRFPointFourQ> process_queryFourQ(std::vector<OPRF::OPRFPointFourQ>& input);
            std::vector<OPRF::OPRFPointFourQ> process_query_threadFourQ(std::vector<OPRF::OPRFPointFourQ>& input);

            DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist);
            std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist);
            void start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            void run(std::vector<Channel>& chlsC);
        private:
            std::vector<LabelMask> hash_table;

            size_t server_set_size_;
            OPRF::OPRFSender DHOPRFSender;
            DPF::dpf_client DPFClient;
        };


    } // namespace Server
    
} // namespace PSI
