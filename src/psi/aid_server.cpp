#include "aid_server.h"
#include "psi/common/thread_pool_mgr.h"
namespace PSI{
    namespace AidServer
    {
        void AidServer::init(const std::vector<std::string>& hash_table_input){
            hash_table = hash_table_input;
        }
        DPF::DPFResponseList AidServer::DPFShare(DPF::DPFKeyList dpfkeylist){
            DPF::DPFResponseList dpf_response_list;
            ThreadPoolMgr tpm;
            size_t task_numbers  = cuckoo::block_num*cuckoo::max_set_size;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), task_numbers);
            std::vector<std::future<void>> futures(task_count);

            
             auto DPFValueEval = [&](size_t blockid,size_t key_id) {
                auto &value_list =   dpf_response_list.at(blockid).DPFValue.at(key_id); 
                const auto& key = dpfkeylist.at(blockid).at(key_id);
                for(size_t idx = 0; idx < cuckoo::block_size; idx++){
                    value_list[idx] = DPFClient.Eval(idx,1,key);
                }
            };

            auto DPFValueWork = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx; idx < task_numbers; idx += step ){
                    DPFValueEval(start_idx/cuckoo::max_set_size,start_idx%cuckoo::max_set_size);
                }
            };

             for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(DPFValueWork, thread_idx, task_count);
            }


            for(size_t block_id = 0; block_id < cuckoo::block_num;block_id++){
                dpf_response_list.at(block_id).cuckoo_table = 
                    std::vector<std::string>(hash_table.begin()+cuckoo::block_size*block_id,hash_table.begin()+cuckoo::block_size*(block_id+1));
            }
            
            for (auto &f : futures) {
                f.get();
            }
            return dpf_response_list;
        }
    } // namespace AidServer
    
} // namespace PSI