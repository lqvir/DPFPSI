#include "aid_server.h"
#include "psi/common/thread_pool_mgr.h"



namespace PSI{
    namespace AidServer
    {



        std::unique_ptr<DPF::DPFResponseList>  AidServer::DPFShareFullEval(const std::unique_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList>& keylist,const std::vector<LabelMask>& hash_table_input){
            return std::move(DPF::DPFClient::FullEval(keylist,hash_table_input,0));
        }
        std::unique_ptr<DPF::DPFResponseList>  AidServer::DPFShareFullEval(const std::unique_ptr<DPF::pcGGM::DPFKeyList>& keylist,const std::vector<LabelMask>& hash_table_input){
            return std::move(DPF::DPFClient::FullEval(keylist,hash_table_input,0));
        }
        void AidServer::run(const std::vector<LabelMask>& input,std::vector<Channel>& chlsC){

            std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_unique<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
            chlsC[0].recv(reinterpret_cast<uint8_t*>(ka.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));
            auto response_a = DPFShareFullEval(ka,input);

            chlsC[0].send(reinterpret_cast<uint8_t*>(response_a.get()),sizeof(PSI::DPF::DPFResponseList));
            
        }

        void AidServer::runSIMD(const std::vector<LabelMask>& input,std::vector<Channel>& chlsC){

            std::unique_ptr<PSI::DPF::pcGGM::DPFKeyList> ka = std::make_unique<PSI::DPF::pcGGM::DPFKeyList>();
            chlsC[0].recv(reinterpret_cast<uint8_t*>(ka.get()),sizeof(PSI::DPF::pcGGM::DPFKeyList));
            auto response_a = DPFShareFullEval(ka,input);

            chlsC[0].send(reinterpret_cast<uint8_t*>(response_a.get()),sizeof(PSI::DPF::DPFResponseList));
            
        }

        void AidServer::DHBasedPSI_start(std::string ServerAddress,std::string ClientAddress){
            IOService ios;
            Session sessionS(ios,ServerAddress,SessionMode::Server);
            Session sessionC(ios,ClientAddress,SessionMode::Server);

            std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
            std::vector<Channel> chlsC(ThreadPoolMgr::GetThreadCount());
            for(auto& chl:chlsC){
                chl = sessionC.addChannel();
            } 
            for(auto& chl:chlsS){
                chl = sessionS.addChannel();
            } 

            std::vector<LabelMask> CuckooTable(cuckoo::table_size);
            chlsS[0].recv(reinterpret_cast<uint8_t*>(CuckooTable.data()),Mask_byte_size*cuckoo::table_size);

            run(CuckooTable,chlsC);
            
            
            for(auto &chl : chlsC){
                chl.close();
            }
            for(auto &chl : chlsS){
                chl.close();
            }
            sessionS.stop();
            sessionC.stop();
            ios.stop();
        }

        void AidServer::DHBased_SIMDDPF_PSI_start(std::string ServerAddress,std::string ClientAddress){
            IOService ios;
            Session sessionS(ios,ServerAddress,SessionMode::Server);
            Session sessionC(ios,ClientAddress,SessionMode::Server);

            std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
            std::vector<Channel> chlsC(ThreadPoolMgr::GetThreadCount());
            for(auto& chl:chlsC){
                chl = sessionC.addChannel();
            } 
            for(auto& chl:chlsS){
                chl = sessionS.addChannel();
            } 

            std::vector<LabelMask> CuckooTable(cuckoo::table_size);
            chlsS[0].recv(reinterpret_cast<uint8_t*>(CuckooTable.data()),Mask_byte_size*cuckoo::table_size);

            runSIMD(CuckooTable,chlsC);
            
            
            for(auto &chl : chlsC){
                chl.close();
            }
            for(auto &chl : chlsS){
                chl.close();
            }
            sessionS.stop();
            sessionC.stop();
            ios.stop();
        }

        void AidServer::GCBasedPSI_start(std::string ServerAddress,std::string ClientAddress){
            IOService ios;
            Session sessionS(ios,ServerAddress,SessionMode::Server);
            Session sessionC(ios,ClientAddress,SessionMode::Server);

            std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
            std::vector<Channel> chlsC(ThreadPoolMgr::GetThreadCount());
            for(auto& chl:chlsC){
                chl = sessionC.addChannel();
            } 
            for(auto& chl:chlsS){
                chl = sessionS.addChannel();
            } 
            std::vector<LabelMask> CuckooTable(cuckoo::table_size);
            chlsS[0].recv(reinterpret_cast<uint8_t*>(CuckooTable.data()),Mask_byte_size*cuckoo::table_size);

            run(CuckooTable,chlsC);
            
            
            for(auto &chl : chlsC){
                chl.close();
            }
            for(auto &chl : chlsS){
                chl.close();
            }
            sessionS.stop();
            sessionC.stop();
            ios.stop();
        }


        void AidServer::GCBased_SIMDDPF_PSI_start(std::string ServerAddress,std::string ClientAddress){
            IOService ios;
            Session sessionS(ios,ServerAddress,SessionMode::Server);
            Session sessionC(ios,ClientAddress,SessionMode::Server);

            std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
            std::vector<Channel> chlsC(ThreadPoolMgr::GetThreadCount());
            for(auto& chl:chlsC){
                chl = sessionC.addChannel();
            } 
            for(auto& chl:chlsS){
                chl = sessionS.addChannel();
            } 

            std::vector<LabelMask> CuckooTable(cuckoo::table_size);
            chlsS[0].recv(reinterpret_cast<uint8_t*>(CuckooTable.data()),Mask_byte_size*cuckoo::table_size);

            runSIMD(CuckooTable,chlsC);
            
            
            for(auto &chl : chlsC){
                chl.close();
            }
            for(auto &chl : chlsS){
                chl.close();
            }
            sessionS.stop();
            sessionC.stop();
            ios.stop();
        }
    } // namespace AidServer
    
} // namespace PSI