#include <stack>

#include "dpf_client.h"
#include "pcGGM.h"
#include "psi/common/thread_pool_mgr.h"
namespace PSI
{
    namespace DPF
    {
        dpf_client::dpf_client(uint8_t party_number):party_number_(party_number){};
        


        uint8_t dpf_client::Eval(std::bitset<DPF_INPUT_BIT_SIZE> x_bitset,const DPFKey& key){
            std::bitset<Lambda> share_i = key.share;
            
            // std::cout<<x_bitset<<std::endl;
            for(size_t i = DPF_INPUT_BIT_SIZE - 1; i > 0 ; i--){
                std::bitset<Lambda> Hs_out;
                Keyed_hash_func(share_i,Hs_out);
                
                if(x_bitset.test(i)){
                    Hs_out ^= share_i;
                }
                if(share_i.test(0)){
                    Hs_out ^= key.cw[i-1];
                }
                share_i = Hs_out;
            }
            // std::cout << share_i << std::endl;
            
            std::bitset<Lambda> high_low;
            std::bitset<Lambda> temp = share_i;
            temp[0]=share_i[0] ^x_bitset[0];
            Keyed_hash_func(temp,high_low);
            // std::cout << "high low " << std::endl << high_low << std::endl;
            std::bitset<Lambda> share_n = high_low;
            if(share_i.test(0)){
                auto mid_string = key.cw_n.to_string();
                if(x_bitset.test(0)){
                    mid_string.erase(mid_string.size()-2,1);
                }
                else{
                    mid_string.pop_back();
                }
                // std::cout << std::bitset<128>(mid_string) << std::endl;

                share_n ^= std::bitset<128>(mid_string);
            }
            // std::cout << share_n << std::endl;
            if(party_number_){
                return (uint8_t)((uint8_t)(-1)*(share_n[1]+share_n[0]*key.cw_n_plus_1));
            }
            else{
                return (uint8_t)((share_n[1]+share_n[0]*key.cw_n_plus_1));
            }

        }
        DPF::DPFResponseList dpf_client::DPFShare(const DPF::DPFKeyList& dpfkeylist,const std::vector<LabelMask>& hash_table){
            
            DPF::DPFResponseList dpf_response_list;
            ThreadPoolMgr tpm;
            size_t task_numbers  = cuckoo::block_num*cuckoo::max_set_size;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), task_numbers);
            std::vector<std::future<void>> futures(task_count);

            
             auto DPFValueEval = [&](size_t blockid,size_t key_id) {
                auto &value =   dpf_response_list.at(blockid).at(key_id); 
                
                const auto& key = dpfkeylist.at(blockid).at(key_id);
                for(size_t idx = 0; idx < cuckoo::block_size; idx++){
                    if(Eval(idx,key)&1){
                        value = xor_LabelMask(value,hash_table.at(cuckoo::get_BinID(idx,blockid)));
#if LogLevel == 0
                
                std::cout << "value before ";
                util::printchar(value.value().data(),Leading_zero_length+Label_byte_size);
                std::cout << "hash ";
                util::printchar(hash_table.at(cuckoo::get_BinID(idx,blockid)).value().data(),Leading_zero_length+Label_byte_size);
                std::cout << "value end ";
                util::printchar(value.value().data(),Leading_zero_length+Label_byte_size);
#endif

                    }
                }
#if LogLevel == 0
                std::cout << "value final ";
                util::printchar(value.value().data(),Leading_zero_length+Label_byte_size);
                std::cout << std::endl;
#endif
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
            
            for (auto &f : futures) {
                f.get();
            }
#if LogLevel == 0
            std::cout << "dpf response list" << std::endl;
            for(auto x : dpf_response_list){
                std::cout << std::endl;
                for(auto y : x){
                    util::printchar(y.value().data(),Leading_zero_length+Label_byte_size);
                }
            }
#endif 

            return dpf_response_list;
        }




        // simply DPFTree 
        pcGGMLeafList dpf_client::DPFGenTree(const DPFKey& key){
            pcGGMLeafList ans;
            size_t cnt = 0;
            std::stack<std::pair<std::bitset<Lambda>,size_t>> DFSStack;

            DFSStack.push(std::make_pair(key.share,0));
            auto mid_string1 = key.cw_n.to_string();
            auto mid_string0 = key.cw_n.to_string();
            
            mid_string1.erase(mid_string1.size()-2,1);
            mid_string0.pop_back();
            
            // std::cout << std::bitset<128>(mid_string0) << std::endl;
            // std::cout << std::bitset<128>(mid_string1) << std::endl;
            
            while (!DFSStack.empty())
            {
                /* code */
                auto now = DFSStack.top();
                DFSStack.pop();

                if(now.second == DPF_INPUT_BIT_SIZE - 1){ // n - 1 break the loop 
                    {
                        std::bitset<Lambda> share_n0 ;
                        std::bitset<Lambda> share_n1 ;
                        Keyed_hash_func(now.first,share_n0);
                        Keyed_hash_func(now.first^std::bitset<Lambda>(1),share_n1);

                        if(now.first.test(0)){
                           

                            share_n0 ^= std::bitset<128>(mid_string0);
                            share_n1 ^= std::bitset<128>(mid_string1);
                        }
                        // std::cout << share_n << std::endl;
                        // if(party_number_){
                        //     ans[cnt] = ((uint8_t)((uint8_t)(-1)*(share_n0[1]+share_n0[0]*key.cw_n_plus_1)) & 1);
                        //     cnt++;
                        //     ans[cnt] = ((uint8_t)((uint8_t)(-1)*(share_n1[1]+share_n1[0]*key.cw_n_plus_1))& 1);
                            
                        // }
                        // else{
                            ans[cnt] = share_n0[1]^(share_n0[0]&key.cw_n_plus_1);

                            // ans[cnt] = ((uint8_t)((share_n0[1]+share_n0[0]*key.cw_n_plus_1))& 1);
                            cnt++;
                            // ans[cnt] = ((uint8_t)((share_n1[1]+share_n1[0]*key.cw_n_plus_1))& 1);
                            ans[cnt] = share_n1[1]^(share_n1[0]&key.cw_n_plus_1);

                        // }
                        // std::cout << (int)ans[cnt-1] << (int)ans[cnt] << std::endl;
                    }

                    cnt++;
                    if(cnt >= cuckoo::block_size){
                        break;
                    }
                }
                else{
                    
                    std::bitset<Lambda> Hs_out;
                    Keyed_hash_func(now.first,Hs_out);
                    
                    if(now.first.test(0)){
                        Hs_out ^= key.cw.at(DPF_INPUT_BIT_SIZE-2-now.second);
                    }
                    DFSStack.push(std::make_pair(Hs_out^now.first,now.second+1));

                    DFSStack.push(std::make_pair(Hs_out,now.second+1));
                }

            }
            
            return ans;
        }

        pcGGMLeafList dpf_client::DPFGenTree(const DPFKeyEarlyTerminal& key){
            pcGGMLeafList ans;
            size_t cnt = 0;
            std::stack<std::pair<std::bitset<Lambda>,size_t>> DFSStack;

            DFSStack.push(std::make_pair(key.share,0));
            auto mid_string1 = key.cw_n.to_string();
            auto mid_string0 = key.cw_n.to_string();
            
            mid_string1.erase(mid_string1.size()-2,1);
            mid_string0.pop_back();
            
            // std::cout << std::bitset<128>(mid_string0) << std::endl;
            // std::cout << std::bitset<128>(mid_string1) << std::endl;
            
            while (!DFSStack.empty())
            {
                /* code */
                auto now = DFSStack.top();
                DFSStack.pop();

                if(now.second == DPF_EAYLY_HIGH - 1){ // n - 1 break the loop 
                    {
                        std::bitset<Lambda> share_n0 ;
                        std::bitset<Lambda> share_n1 ;
                        Keyed_hash_func(now.first,share_n0);
                        Keyed_hash_func(now.first^std::bitset<Lambda>(1),share_n1);

                        if(now.first.test(0)){
                           

                            share_n0 ^= std::bitset<128>(mid_string0);
                            share_n1 ^= std::bitset<128>(mid_string1);
                        }
                   
                        std::bitset<DPF_COMPRESS_NODES_NUMBER> out;
                        Convert_to_G(share_n0,out);
                        // std::cout << "tree " << std::endl;
                        for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                            ans[cnt++] = out[idx]^(key.cw_n_1[idx]&share_n0[0]);

                            // std::cout << out[idx] <<' '<< key.cw_n_1[idx] <<' '<< ans[idx] << std::endl;
                        }
                        if(cnt >= cuckoo::block_size){
                            break;
                        }
                        Convert_to_G(share_n1,out);
                        for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                            ans[cnt++] = out[idx]^(key.cw_n_1[idx]&share_n1[0]);
                        }
                        if(cnt >= cuckoo::block_size){
                            break;
                        }
                    }


                }
                else{
                    
                    std::bitset<Lambda> Hs_out;
                    Keyed_hash_func(now.first,Hs_out);
                    
                    if(now.first.test(0)){
                        Hs_out ^= key.cw.at(DPF_EAYLY_HIGH-2-now.second);
                    }
                    DFSStack.push(std::make_pair(Hs_out^now.first,now.second+1));

                    DFSStack.push(std::make_pair(Hs_out,now.second+1));
                }

            }
            
            return ans;
        }




        std::shared_ptr<DPFResponseList> dpf_client::FullEval(const DPFKeyList& dpfkeylist,const std::vector<LabelMask>& hash_table){

            std::shared_ptr<DPFResponseList> dpf_response_list = std::make_shared<DPFResponseList>();
            ThreadPoolMgr tpm;
            size_t task_numbers  = cuckoo::block_num*cuckoo::max_set_size;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), task_numbers);
            std::vector<std::future<void>> futures(task_count);
            auto FullEvalWorker = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx; idx < task_numbers; idx += step ){
                    auto blockid = idx/cuckoo::max_set_size;
                    auto keyid = idx%cuckoo::max_set_size;
                    auto &value =   dpf_response_list->at(blockid).at(keyid); 
                    const auto& key = dpfkeylist.at(blockid).at(keyid);

                    auto LeafList = DPFGenTree(key);
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

            return dpf_response_list;

        }

        std::shared_ptr<DPFResponseList> dpf_client::FullEval(const std::shared_ptr<DPFKeyEarlyTerminalList> dpfkeylist,const std::vector<LabelMask>& hash_table){
            std::shared_ptr<DPFResponseList> dpf_response_list = std::make_shared<DPFResponseList>();
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
                    auto LeafList = DPFGenTree(key);
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

            return dpf_response_list;

        }

        std::shared_ptr<DPFResponseList> dpf_client::FullEval(const std::shared_ptr<DPFKeyEarlyTerminal_ByArrayList> dpfkeylist,const std::vector<LabelMask>& hash_table){
            std::shared_ptr<DPFResponseList> dpf_response_list = std::make_shared<DPFResponseList>();
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
            return dpf_response_list;
        }


    } // namespace DPF
    
} // namespace PSI
