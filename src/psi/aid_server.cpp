#include "aid_server.h"
#include "psi/common/thread_pool_mgr.h"



namespace PSI{
    namespace AidServer
    {



        std::shared_ptr<DPF::DPFResponseList>  AidServer::DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.FullEval(keylist,hash_table_input);
        }

        void AidServer::run(const std::vector<LabelMask>& input,std::vector<Channel>& chlsC){

            std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
            
            chlsC[0].recv(reinterpret_cast<uint8_t*>(ka.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));
            auto response_a = DPFShareFullEval(ka,input);

            chlsC[0].send(reinterpret_cast<uint8_t*>(response_a.get()),sizeof(PSI::DPF::DPFResponseList));
            
    

        }

        void AidServer::start(std::string SelfAddress,std::string ServerAddress){
            IOService ios;
            Session sessionS(ios,SelfAddress,SessionMode::Server);
            Session sessionC(ios,ServerAddress,SessionMode::Server);

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
    } // namespace AidServer
    
} // namespace PSI