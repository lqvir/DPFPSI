#pragma once

#include "oprf/dhoprf_receiver.h"
#include "dpf/dpf_server.h"
#include "param.h"

#include <vector>
#include <span>
#include <unordered_map>

namespace PSI{
    namespace Client
    {
        

        class PSIClient{
            public:
                PSIClient() = default;
                PSIClient(size_t client_set_size):client_set_size_(client_set_size){};
                std::vector<std::string> OPRFQuery(const std::vector<Item>& input);
                std::vector<std::string> OPRFResponse(const std::vector<std::string>& response);
                void Cuckoo_All_location(const std::vector<std::string>& oprf_input);
                void DPFGen(
                    DPF::DPFKeyList& Ks,DPF::DPFKeyList& Ka
                );
                void DPFGen(
                    DPF::DPFKeyEarlyTerminalList& Ks,
                    DPF::DPFKeyEarlyTerminalList& Ka
                );



                void DictGen(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA);

                void InsectionCheck(const std::vector<std::string>& oprf_input,const std::vector<Item>& input);
            private:
                OPRF::OPRFReceiver DHOPRFReceiver;
                size_t client_set_size_;
                std::vector<std::vector<size_t>> IndexSets_by_block; 
                DPF::dpf_server DPFServer;
                std::unordered_map<size_t,LabelMask> Dict;
        };

    } // namespace Client
}