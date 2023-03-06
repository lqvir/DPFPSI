#include "server.h"
#include "kuku/kuku.h"
#include "cuckoo_hash/cuckoo.h"
#include "psi/common/thread_pool_mgr.h"
#include "psi/common/utils.h"
namespace PSI
{
    namespace Server
    {
        std::vector<std::string> PSIServer::init(const std::vector<Item>& input,const std::vector<PSI::Label>& input_Label){
            DHOPRFSender.init();
            auto output = DHOPRFSender.ComputeHashes(input);
            std::cout << "oprf_value" <<std::endl;;

            for(auto x : output){
                util::printchar((uint8_t*)x.data(),x.size());
            }

            kuku::KukuTable cuckoo_table(
                cuckoo::table_size,      // Size of the hash table
                0,                                      // Not using a stash
                cuckoo::hash_func_count, // Number of hash functions
                { 0, 0 },                               // Hardcoded { 0, 0 } as the seed
                cuckoo::cuckoo_table_insert_attempts,           // The number of insertion attempts
                { 0, 0 });
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++){
                const auto &item = output[item_idx].substr(0,16);
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
                const auto &item = output[item_idx].substr(0,16);

                auto item_loc = cuckoo_table.query(kuku::make_item((uint8_t*)item.data())).location();


                hash_table[item_loc] = output[item_idx].substr(16,leading_zero_length+Label_byte_size);
                util::printchar((uint8_t*)input[item_idx].get_as<uint8_t>().data(),(size_t)16);
                std::cout << item_loc << std::endl;
                util::printchar((uint8_t*)hash_table[item_loc].data(),leading_zero_length+Label_byte_size);
                util::printchar((uint8_t*)input_Label[item_idx].data(),Label_byte_size);
                
                util::xor_buffers((uint8_t*)(hash_table[item_loc].data()+leading_zero_length),(uint8_t*)input_Label[item_idx].data(),Label_byte_size);
                // itt.table_idx_to_item_idx_[item_loc.location()] = item_idx;
            }

            // for(size_t idx = 0 ; idx < cuckoo::table_size ; idx ++){
            //     std::cout << idx;
            //     util::printchar((uint8_t*)hash_table[idx].data(),leading_zero_length+Label_byte_size);
            // }
            

            return hash_table;

        }

        std::vector<std::string>  PSIServer::process_query(const std::vector<std::string>& input){
            return DHOPRFSender.ProcessQeries(input);
        }

        DPF::DPFResponseList PSIServer::DPFShare(DPF::DPFKeyList dpfkeylist){
            DPF::DPFResponseList dpf_response_list;
            ThreadPoolMgr tpm;
            size_t task_numbers  = cuckoo::block_num*cuckoo::max_set_size;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), task_numbers);
            std::vector<std::future<void>> futures(task_count);

            
             auto DPFValueEval = [&](size_t blockid,size_t key_id) {
                // std::cout << blockid << " " << key_id << std::endl;
                auto &value_list =   dpf_response_list.at(blockid).DPFValue.at(key_id); 
                const auto& key = dpfkeylist.at(blockid).at(key_id);
                for(size_t idx = 0; idx < cuckoo::block_size; idx++){
                    value_list[idx] = DPFClient.Eval(idx,0,key);
                }
            };

            auto DPFValueWork = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx; idx < task_numbers; idx += step ){
                    DPFValueEval(idx/cuckoo::max_set_size,idx%cuckoo::max_set_size);
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

    } // namespace Server
    
} // namespace PSI
