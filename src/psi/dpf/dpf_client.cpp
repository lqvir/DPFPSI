#include <stack>

#include "dpf_client.h"
#include "psi/common/thread_pool_mgr.h"
namespace PSI
{
    namespace DPF
    {
       
        std::unique_ptr<DPFResponseList> DPFClient::FullEval(const std::unique_ptr<DPFKeyEarlyTerminal_ByArrayList>& dpfkeylist,const std::vector<LabelMask>& hash_table,size_t party_number){
            std::unique_ptr<DPFResponseList> dpf_response_list = std::make_unique<DPFResponseList>();
            ThreadPoolMgr tpm;
            size_t task_numbers  = cuckoo::block_num*cuckoo::max_set_size;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), task_numbers);
            std::vector<std::future<void>> futures(task_count); 
            auto FullEvalWorker = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx; idx < task_numbers; idx += step ){
                    auto blockid = idx/cuckoo::max_set_size;
                    auto keyid = idx%cuckoo::max_set_size;
                    auto &value =   dpf_response_list->at(blockid).at(keyid); 
                    const auto& key = dpfkeylist->at(blockid).at(keyid);
                    auto LeafList = pcGGM::GenTree(key);
                    for(size_t idx = 0; idx < cuckoo::block_size; idx++){

                        if( LeafList.test(idx)){
                            value = xor_LabelMask(value,hash_table.at(cuckoo::get_BinID(idx,blockid)));
                        }

                    }
                    
                }
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(FullEvalWorker, thread_idx, task_count);
            }
            
            for (auto &f : futures) {
                f.get();
            }
            return std::move(dpf_response_list);
        }
       

    } // namespace DPF
    
} // namespace PSI
