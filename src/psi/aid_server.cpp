#include "aid_server.h"
#include "psi/common/thread_pool_mgr.h"
namespace PSI{
    namespace AidServer
    {
        void AidServer::init(const std::vector<LabelMask>& hash_table_input){
            hash_table = hash_table_input;
        }
        DPF::DPFResponseList AidServer::DPFShare(const DPF::DPFKeyList& keylist,const std::vector<LabelMask>& hash_table_input){
            return DPFClient.DPFShare(keylist,hash_table_input);
        }
    } // namespace AidServer
    
} // namespace PSI