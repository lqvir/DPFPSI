#pragma once

#include "oprf/dhoprf_receiver.h"
#include "oprf/GCOPRF_receiver.h"
#include "dpf/dpf_server.h"
#include "param.h"

#include <vector>
#include <span>
#include <memory>

#include <unordered_map>
#include "common/Network/Session.h"
#include "common/Network/IOService.h"
#include "common/Network/Channel.h"
namespace PSI{
    namespace Client
    {
        

        class PSIClient{
            public:
                PSIClient() = delete;

                PSIClient(size_t client_set_size,droidCrypto::ChannelWrapper& chan):client_set_size_(client_set_size),GCOPRFReceiver(chan){};
                
                
                std::vector<DHOPRF::OPRFPoint> OPRFQuery(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFPoint> OPRFQueryThread(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponse(const std::vector<DHOPRF::OPRFPoint>& response);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseThread(const std::vector<DHOPRF::OPRFPoint>& response);

                std::vector<DHOPRF::OPRFPointFourQ> OPRFQueryFourQ(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFPointFourQ> OPRFQueryThreadFourQ(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseThreadFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response);
               
                // Generate Simple Hashing table for DH based OPRF               
                void Cuckoo_All_location(const std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input);
                // Generate Simple Hashing table for GC based OPRF
                void Cuckoo_All_location(const std::unique_ptr<std::vector<GCOPRF::GCOPRFValue>>& oprf_input);

                // Generate DPF query with Early Termination Opt
                void DPFGen(
                    const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>& Ks,
                    const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>& Ka
                );
                // Generate DPF query
                void DPFGen(
                    const std::unique_ptr<PSI::DPF::pcGGM::DPFKeyList>& Ks,
                    const std::unique_ptr<PSI::DPF::pcGGM::DPFKeyList>& Ka
                );
                // Generate DPF query
                void DictGen(
                    const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromS,
                    const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromA
                );

                // Check Intersection as receiving the OPRF Response for DH LPSI 
                void InsectionCheck(std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input,const std::vector<Item>& input);
                
                // Check Intersection as receiving the OPRF Response for GC LPSI
                void InsectionCheck(const std::unique_ptr<std::vector<PSI::GCOPRF::GCOPRFValue>>& oprf_input,const std::vector<Item>& input);
                void run(std::string ServerAddress,std::string AidServerAddress,std::unique_ptr<std::vector<Item>> input);
               
                // All procedure of DH Based LPSI 
                void DHBasedPSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
                
                // SIMD Opt for all procedure of DH based LPSI
                void DHBased_SIMDDPF_PSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
               
                // All procedure of GC based LPSI
                void GCBasedPSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
                
                // SIMD Opt for all procedure of GC based LPSI
                void GCBased_SIMDDPF_PSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
                
            private:
                DHOPRF::OPRFReceiver DHOPRFReceiver;
                GCOPRF::OPRFReciver GCOPRFReceiver;
                size_t client_set_size_;
                std::vector<std::vector<size_t>> IndexSets_by_block; 
                std::unordered_map<size_t,LabelMask> Dict;
                
        };

    } // namespace Client
}