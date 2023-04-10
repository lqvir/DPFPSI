#include "server.h"
#include "kuku/kuku.h"
#include "cuckoo_hash/cuckoo.h"
#include "psi/common/thread_pool_mgr.h"
#include "psi/common/utils.h"
#include "psi/common/stopwatch.h"

namespace PSI
{
    namespace Server
    {
        std::vector<LabelMask> PSIServer::init(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label){
            PSI::StopWatch clocks("init");
            clocks.setDurationStart("oprf");
            DHOPRFSender.init();
            auto output = DHOPRFSender.ComputeHashes(input);
            clocks.setDurationEnd("oprf");
            std::cout << __LINE__ << std::endl;

            clocks.printDurationRecord();
            std::cout << __LINE__ << std::endl;
            
#if  LogLevel == 0



            std::cout << "oprf_value" <<std::endl;;

            for(auto x : output){
                util::printchar((uint8_t*)x.data(),x.size());
            }
#endif 
            kuku::KukuTable cuckoo_table(
                cuckoo::table_size,      // Size of the hash table
                0,                                      // Not using a stash
                cuckoo::hash_func_count, // Number of hash functions
                { 0, 0 },                               // Hardcoded { 0, 0 } as the seed
                cuckoo::cuckoo_table_insert_attempts,           // The number of insertion attempts
                { 0, 0 });
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++){
                const auto &item = output[item_idx];
                if (!cuckoo_table.insert(kuku::make_item((uint8_t*)item.data()))) {
                    // Insertion can fail for two reasons:
                    //
                    //     (1) The item was already in the table, in which case the "leftover
                    //     item" is empty; (2) Cuckoo hashing failed due to too small table or
                    //     too few hash functions.
                    //
                    // In case (1) simply move on to the next item and log this issue. Case (2)
                    // is a critical issue so we throw and exception.
                    if (cuckoo_table.is_empty_item(cuckoo_table.leftover_item())) {
                        
                        std::cout <<"Skipping repeated insertion of items["
                            << item_idx <<"]:" ;
                        util::printchar((uint8_t*)item.data(),16);
                    } else {
                         std::cout <<
                            "Failed to insert items["
                            << item_idx << "] "
                            << " cuckoo table fill-rate: " << cuckoo_table.fill_rate();
                        throw std::runtime_error("failed to insert item into cuckoo table");
                    }
                }

            }
            hash_table.resize(cuckoo::table_size);
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++) {
                const auto &item = output[item_idx];

                auto item_loc = cuckoo_table.query(kuku::make_item((uint8_t*)item.data())).location();
                memcpy(hash_table[item_loc].value().data(),output[item_idx].data()+16,Leading_zero_length+Label_byte_size);
                // hash_table[item_loc] = std::array<uint8_t,24>(output[item_idx].begin()+16,output[item_idx].end());
                // hash_table[item_loc] = output[item_idx].data()(16,Leading_zero_length+Label_byte_size);
#if LogLevel == 0           
                util::printchar((uint8_t*)input[item_idx].get_as<uint8_t>().data(),(size_t)Item_byte_size);
                std::cout << item_loc << std::endl;
                util::printchar((uint8_t*)hash_table[item_loc].value().data(),Leading_zero_length+Label_byte_size);
                util::printchar((uint8_t*)input_Label[item_idx].get_as<uint8_t>().data(),Label_byte_size);
#endif            
                
                util::xor_buffers((uint8_t*)(hash_table[item_loc].value().data()+Leading_zero_length),(uint8_t*)input_Label[item_idx].get_as<uint8_t>().data(),Label_byte_size);
                // itt.table_idx_to_item_idx_[item_loc.location()] = item_idx;
            }
#if LogLevel == 0
            for(size_t idx = 0 ; idx < cuckoo::table_size ; idx ++){
                std::cout << idx;
                util::printchar((uint8_t*)hash_table[idx].value().data(),Leading_zero_length+Label_byte_size);
            }
#endif            

            return hash_table;

        }
        std::vector<LabelMask> PSIServer::init_FourQ(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label){
      
            DHOPRFSender.init();
            auto output = DHOPRFSender.ComputeHashesFourQ(input);
        
            std::cout << __LINE__ << std::endl;

#if  LogLevel == 0



            std::cout << "oprf_value" <<std::endl;;

            for(auto x : output){
                util::printchar((uint8_t*)x.data(),x.size());
            }
#endif 
            kuku::KukuTable cuckoo_table(
                cuckoo::table_size,      // Size of the hash table
                0,                                      // Not using a stash
                cuckoo::hash_func_count, // Number of hash functions
                { 0, 0 },                               // Hardcoded { 0, 0 } as the seed
                cuckoo::cuckoo_table_insert_attempts,           // The number of insertion attempts
                { 0, 0 });
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++){
                const auto &item = output[item_idx];
                if (!cuckoo_table.insert(kuku::make_item((uint8_t*)item.data()))) {
                    // Insertion can fail for two reasons:
                    //
                    //     (1) The item was already in the table, in which case the "leftover
                    //     item" is empty; (2) Cuckoo hashing failed due to too small table or
                    //     too few hash functions.
                    //
                    // In case (1) simply move on to the next item and log this issue. Case (2)
                    // is a critical issue so we throw and exception.
                    if (cuckoo_table.is_empty_item(cuckoo_table.leftover_item())) {
                        
                        std::cout <<"Skipping repeated insertion of items["
                            << item_idx <<"]:" ;
                        util::printchar((uint8_t*)item.data(),16);
                    } else {
                         std::cout <<
                            "Failed to insert items["
                            << item_idx << "] "
                            << " cuckoo table fill-rate: " << cuckoo_table.fill_rate();
                        throw std::runtime_error("failed to insert item into cuckoo table");
                    }
                }

            }
            hash_table.resize(cuckoo::table_size);
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++) {
                const auto &item = output[item_idx];

                auto item_loc = cuckoo_table.query(kuku::make_item((uint8_t*)item.data())).location();
                // memcpy(hash_table[item_loc].value().data(),output[item_idx].data()+16,Leading_zero_length+Label_byte_size);
                hash_table[item_loc] = gsl::make_span(output[item_idx]).subspan(16,Mask_byte_size);
                // hash_table[item_loc] = std::array<uint8_t,24>(output[item_idx].begin()+16,output[item_idx].end());
                // hash_table[item_loc] = output[item_idx].data()(16,Leading_zero_length+Label_byte_size);
#if LogLevel == 0           
                util::printchar((uint8_t*)input[item_idx].get_as<uint8_t>().data(),(size_t)Item_byte_size);
                std::cout << item_loc << std::endl;
                util::printchar((uint8_t*)hash_table[item_loc].value().data(),Leading_zero_length+Label_byte_size);
                util::printchar((uint8_t*)input_Label[item_idx].get_as<uint8_t>().data(),Label_byte_size);
#endif            
                
                util::xor_buffers((uint8_t*)(hash_table[item_loc].value().data()+Leading_zero_length),(uint8_t*)input_Label[item_idx].get_as<uint8_t>().data(),Label_byte_size);
                // itt.table_idx_to_item_idx_[item_loc.location()] = item_idx;
            }
#if LogLevel == 0
            for(size_t idx = 0 ; idx < cuckoo::table_size ; idx ++){
                std::cout << idx;
                util::printchar((uint8_t*)hash_table[idx].value().data(),Leading_zero_length+Label_byte_size);
            }
#endif            

            return hash_table;

        }
        std::vector<OPRF::OPRFPointOpenSSL>  PSIServer::process_query(const std::vector<OPRF::OPRFPointOpenSSL>& input){
            return DHOPRFSender.ProcessQueries(input);
        }
        std::vector<OPRF::OPRFPointOpenSSL>  PSIServer::process_query_thread(const std::vector<OPRF::OPRFPointOpenSSL>& input){
            return DHOPRFSender.ProcessQueriesThread(input);
        }

        std::vector<OPRF::OPRFPointFourQ>  PSIServer::process_queryFourQ(std::vector<OPRF::OPRFPointFourQ>& input){
            return DHOPRFSender.ProcessQueriesFourQ(input);
        }
        std::vector<OPRF::OPRFPointFourQ>  PSIServer::process_query_threadFourQ(std::vector<OPRF::OPRFPointFourQ>& input){
            return DHOPRFSender.ProcessQueriesThreadFourQ(input);
        }
        DPF::DPFResponseList PSIServer::DPFShare(const DPF::DPFKeyList& keylist){
            return DPFClient.DPFShare(keylist,hash_table);
        }

        std::shared_ptr<DPF::DPFResponseList> PSIServer::DPFShareFullEval(const DPF::DPFKeyList& keylist){
            return DPFClient.FullEval(keylist,hash_table);
        }
        std::shared_ptr<DPF::DPFResponseList> PSIServer::DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist){
            return DPFClient.FullEval(keylist,hash_table);
        }
        std::shared_ptr<DPF::DPFResponseList> PSIServer::DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist){
            return DPFClient.FullEval(keylist,hash_table);
        }

        void PSIServer::start(std::string SelfAddress,std::string AidAddress,const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label){
            StopWatch clocks("PSIServer");
            IOService ios;
            Session SessionC(ios,SelfAddress,SessionMode::Server);
            Session SessionA(ios,AidAddress,SessionMode::Client);

            std::vector<Channel> chlsC(ThreadPoolMgr::GetThreadCount());
            std::vector<Channel> chlsA(ThreadPoolMgr::GetThreadCount());
            for(auto &chl : chlsC){
                chl = SessionC.addChannel(); 
            }
            for(auto &chl : chlsA){
                chl = SessionA.addChannel(); 
            }
            clocks.setDurationStart("Offline");

            init_FourQ(input,input_Label);
            chlsA[0].send(reinterpret_cast<uint8_t*>(hash_table.data()),Mask_byte_size*cuckoo::table_size);
            clocks.setDurationEnd("Offline");

            run(chlsC);
            for(auto &chl : chlsC){
                chl.close();
            }
            for(auto &chl : chlsA){
                chl.close();
            }
            clocks.printDurationRecord();
            SessionA.stop();
            SessionC.stop();
            ios.stop();
        }
        

        void PSIServer::run(std::vector<Channel>& chlsC){

            std::vector<OPRF::OPRFPointFourQ> query(server_set_size_);
            chlsC[0].recv(query);
            auto response = process_query_threadFourQ(query);
            chlsC[1].send(response);

            std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ks = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
            chlsC[0].recv(reinterpret_cast<uint8_t*>(ks.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));
            auto response_s = DPFShareFullEval(ks);
            chlsC[1].send(reinterpret_cast<uint8_t*>(response_s.get()),sizeof(PSI::DPF::DPFResponseList));



        }
    } // namespace Server
    
} // namespace PSI
