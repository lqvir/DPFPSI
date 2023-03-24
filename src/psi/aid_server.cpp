#include "aid_server.h"
#include "psi/common/thread_pool_mgr.h"
namespace PSI{
    namespace AidServer
    {

        DPF::DPFResponseList AidServer::DPFShare(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.DPFShare(keylist,hash_table_input);
        }
         std::shared_ptr<DPF::DPFResponseList> AidServer::DPFShareFullEval(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.FullEval(keylist,hash_table_input);
        }
         std::shared_ptr<DPF::DPFResponseList> AidServer::DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminalList> keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.FullEval(keylist,hash_table_input);
        }
        // std::unique_ptr<DPF::DPFResponseList>  AidServer::DPFShareFullEval(const std::unique_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist,const std::vector<LabelMask>& hash_table_input){
        //     return std::move(DPFClient.FullEval(std::move(keylist,hash_table_input));
        // }

        std::shared_ptr<DPF::DPFResponseList>  AidServer::DPFShareFullEval(const std::shared_ptr<DPF::DPFKeyEarlyTerminal_ByArrayList> keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.FullEval(keylist,hash_table_input);
        }
    } // namespace AidServer
    
} // namespace PSI