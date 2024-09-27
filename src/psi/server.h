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
            

            // Init Phase for the PSIServer with Garble Circuit Based OPRF
            std::vector<LabelMask> init_GC(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            
            // Init Phase for the PSIServer with DH based OPRF
            std::vector<LabelMask> init_FourQ(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            
            
            // Execution Phase for PSIServer with DH based OPRF
            void runDH(std::vector<Channel>& chlsC);
            
            // SIMD Improvement for PSI Server with DH based OPRF 
            void runDHSIMD(std::vector<Channel>& chlsC);
            
            // Execution Phase for PSIServer with GC based OPRF
            void runGC(std::vector<Channel>& chlsC);

            // SIMD Improvement for PSIServer with GC based OPRF
            void runGCSIMD(std::vector<Channel>& chlsC);

            // Process DPF Query with OpenSSL
            std::vector<DHOPRF::OPRFPoint> process_query(const std::vector<DHOPRF::OPRFPoint>& input);
            
            // Process DPF Query with OpenSSL and multithreading
            std::vector<DHOPRF::OPRFPoint> process_query_thread(const std::vector<DHOPRF::OPRFPoint>& input);
            
            // Process DPF Query with FourQLib
            std::vector<DHOPRF::OPRFPointFourQ> process_queryFourQ(std::vector<DHOPRF::OPRFPointFourQ>& input);
            
            // Process DPF Query with FourQLib and multithreading
            std::vector<DHOPRF::OPRFPointFourQ> process_query_threadFourQ(std::vector<DHOPRF::OPRFPointFourQ>& input);

            // DPF::DPFResponseList DPFShare(const DPF::DPFKeyList& keylist);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const DPF::DPFKeyList& keylist);
            // std::shared_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist);
            
            // DPF Full Evaluate Optimization with Early Termination
            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::unique_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList>& keylist);
            
            // DPF Full Evaluate Optimization
            std::unique_ptr<DPF::DPFResponseList> DPFShareFullEval(const std::unique_ptr<DPF::pcGGM::DPFKeyList>& keylist);
            
            
            // All Procedure for DH based LPSI
            void DHBasedPSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            
            // SIMD Improvement for DH based LPSI
            void DHBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            
            // All Procedure for GC based LPSI
            void GCBasedPSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            
            // SIMD Improvement for GC based LPSI
            void GCBased_SIMDDPF_PSI_start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label);
            



        private:
            // Cuckoo Hash Table
            std::vector<LabelMask> hash_table;
            // Communication Cost by byte
            size_t Coummunication_Cost;
            size_t server_set_size_;
            DHOPRF::OPRFSender DHOPRFSender;
            GCOPRF::OPRFSender GCOPRFSender;
        };


    } // namespace Server
    
} // namespace PSI
