#pragma once


#include "oprf/dhoprf_sender.h"
#include "oprf/GCOPRF_sender.h"
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

            PSIServer(size_t server_set_size,droidCrypto::ChannelWrapper& chan):server_set_size_(server_set_size),GCOPRFSender(chan){
            };
            std::vector<LabelMask> init_GC(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            std::vector<LabelMask> init_FourQ(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            std::vector<DHOPRF::OPRFPoint> process_query(const std::vector<DHOPRF::OPRFPoint>& input);
            std::vector<DHOPRF::OPRFPoint> process_query_thread(const std::vector<DHOPRF::OPRFPoint>& input);
            std::vector<DHOPRF::OPRFPointFourQ> process_queryFourQ(std::vector<DHOPRF::OPRFPointFourQ>& input);
            std::vector<DHOPRF::OPRFPointFourQ> process_query_threadFourQ(std::vector<DHOPRF::OPRFPointFourQ>& input);

            // DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist);
            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::unique_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList>& keylist);
            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::unique_ptr<DPF::pcGGM::DPFKeyList>& keylist);
            void DHBasedPSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            void DHBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            void GCBasedPSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            void GCBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            void runDH(std::vector<Channel>& chlsC);
            void runDHSIMD(std::vector<Channel>& chlsC);
            void runGC(std::vector<Channel>& chlsC);
            void runGCSIMD(std::vector<Channel>& chlsC);

        private:
            std::vector<LabelMask> hash_table;
            size_t Coummunication_Cost;
            size_t server_set_size_;
            DHOPRF::OPRFSender DHOPRFSender;
            GCOPRF::OPRFSender GCOPRFSender;
        };


    } // namespace Server
    
} // namespace PSI
