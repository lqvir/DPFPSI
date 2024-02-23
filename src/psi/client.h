#pragma once

#include "oprf/dhoprf_receiver.h"
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
                PSIClient() = default;
                PSIClient(size_t client_set_size):client_set_size_(client_set_size){};
                std::vector<DHOPRF::OPRFPoint> OPRFQuery(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFPoint> OPRFQueryThread(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponse(const std::vector<DHOPRF::OPRFPoint>& response);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseThread(const std::vector<DHOPRF::OPRFPoint>& response);

                std::vector<DHOPRF::OPRFPointFourQ> OPRFQueryFourQ(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFPointFourQ> OPRFQueryThreadFourQ(const std::vector<Item>& input);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response);
                std::vector<DHOPRF::OPRFValueOpenssL> OPRFResponseThreadFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response);
                void Cuckoo_All_location(const std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input);
                // void DPFGen(
                //     DPF::DPFKeyList& Ks,DPF::DPFKeyList& Ka
                // );
                // void DPFGen(
                //     std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ks,
                //     std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ka
                // );
              void DPFGen(
                    const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>& Ks,
                    const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>& Ka
                );


                void DictGen(
                    const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromS,
                    const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromA
                );

                void InsectionCheck(std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input,const std::vector<Item>& input);

                void run(std::string ServerAddress,std::string AidServerAddress,std::unique_ptr<std::vector<Item>> input);
                void run(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
                
            private:
                DHOPRF::OPRFReceiver DHOPRFReceiver;
                size_t client_set_size_;
                std::vector<std::vector<size_t>> IndexSets_by_block; 
                std::unordered_map<size_t,LabelMask> Dict;
                
        };

    } // namespace Client
}