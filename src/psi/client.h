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
                std::vector<OPRF::OPRFPointOpenSSL> OPRFQuery(const std::vector<Item>& input);
                std::vector<OPRF::OPRFPointOpenSSL> OPRFQueryThread(const std::vector<Item>& input);
                std::vector<OPRF::OPRFValueOpenssL> OPRFResponse(const std::vector<OPRF::OPRFPointOpenSSL>& response);
                std::vector<OPRF::OPRFValueOpenssL> OPRFResponseThread(const std::vector<OPRF::OPRFPointOpenSSL>& response);

                std::vector<OPRF::OPRFPointFourQ> OPRFQueryFourQ(const std::vector<Item>& input);
                std::vector<OPRF::OPRFPointFourQ> OPRFQueryThreadFourQ(const std::vector<Item>& input);
                std::vector<OPRF::OPRFValueOpenssL> OPRFResponseFourQ(const std::vector<OPRF::OPRFPointFourQ>& response);
                std::vector<OPRF::OPRFValueOpenssL> OPRFResponseThreadFourQ(const std::vector<OPRF::OPRFPointFourQ>& response);
                void Cuckoo_All_location(const std::vector<OPRF::OPRFValueOpenssL>& oprf_input);
                void DPFGen(
                    DPF::DPFKeyList& Ks,DPF::DPFKeyList& Ka
                );
                void DPFGen(
                    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ks,
                    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ka
                );
              void DPFGen(
                    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> Ks,
                    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> Ka
                );


                void DictGen(
                    std::shared_ptr<DPF::DPFResponseList> ResponseListFromS,
                    std::shared_ptr<DPF::DPFResponseList> ResponseListFromA
                );

                void InsectionCheck(std::vector<OPRF::OPRFValueOpenssL>& oprf_input,const std::vector<Item>& input);

                void run(std::string ServerAddress,std::string AidServerAddress,std::unique_ptr<std::vector<Item>> input);
                void run(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input);
                
            private:
                OPRF::OPRFReceiver DHOPRFReceiver;
                size_t client_set_size_;
                std::vector<std::vector<size_t>> IndexSets_by_block; 
                DPF::dpf_server DPFServer;
                std::unordered_map<size_t,LabelMask> Dict;
                
        };

    } // namespace Client
}