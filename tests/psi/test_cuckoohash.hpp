#pragma once
#include <fstream>
#include "kuku/kuku.h"
#include "psi/common/thread_pool_mgr.h"

inline bool cuckoo_test_worker(size_t table_size,size_t item_num,uint8_t* temp){
    
    kuku::KukuTable cuckoo_table(
        table_size,      // Size of the hash table
        0,                                      // Not using a stash
        cuckoo::hash_func_count, // Number of hash functions
        { 0, 0 },                               // Hardcoded { 0, 0 } as the seed
        cuckoo::cuckoo_table_insert_attempts,           // The number of insertion attempts
        { 0, 0 });
    RAND_bytes(temp,item_num*16);
    uint8_t* temp_use = temp;
    size_t cnt = 0;
    for(size_t idx =0 ; idx < item_num; idx ++ ){
        
        if(!cuckoo_table.insert(kuku::make_item(temp_use))){
            if (cuckoo_table.is_empty_item(cuckoo_table.leftover_item())) {
                        
                std::cout <<"Skipping repeated insertion of items["
                    << idx <<"]:" ;
            } else {
                    // std::cout <<
                    // "Failed to insert items["
                    // << idx << "] "
                    // << " cuckoo table fill-rate: " << cuckoo_table.fill_rate() << std::endl 
                    // <<("failed to insert item into cuckoo table") << std::endl;;
                    return true;
            }

        }
    

            temp_use+=8;
            // PSI::util::printchar(temp,16);

    }
    return false;

    
}




inline void cuckoo_test(){

    uint8_t temp[16];
    RAND_bytes(temp,16);

    size_t task_cnt = ThreadPoolMgr::GetThreadCount();
    size_t one_seg_num = 32768;
    size_t cuckoo_table_size = 1048576;


    ThreadPoolMgr tpm;
    std::vector<std::future<void> > threads_vec(task_cnt);
    std::cout << one_seg_num << std::endl;
    // std::vector<float> rates {1.20,1.205,1.21,1.215,1.22,1.223,1.225,1,227,1.230};
    std::vector<float> rates {1.16,1.165,1.17};
    // std::vector<float> rates {1.135,1.155,1.157,1.16,1.17};
    
    // std::vector<float> rates {1.15};


    size_t test_size = rates.size();

    std::vector<size_t> records(test_size);
    std::vector<size_t> records24(test_size);
    std::vector<size_t> records16(test_size);

    auto sege = [&](size_t ridx){
        uint8_t* randomdata = new uint8_t [cuckoo_table_size * 16];
        for(size_t rounds = 0; rounds < one_seg_num ; rounds++){
            if(cuckoo_test_worker(cuckoo_table_size*rates[ridx],cuckoo_table_size,randomdata)) records[ridx]++;
        }
        delete [] randomdata;
    };

    // auto sege24 = [&](size_t ridx){

    //     for(size_t rounds = 0,start = 0; rounds <= one_seg_num ; rounds++,start++){
    //         if(start%16 == 0) 
    //         if(cuckoo_test_worker(16777216*rates[ridx],16777216,)) records24[ridx]++;
    //     }

    // };
    
    // auto sege16 = [&](size_t ridx){

    //     for(size_t rounds = 0; rounds <= one_seg_num ; rounds++){
    //         if(cuckoo_test_worker(65536*rates[ridx],65536,rounds)) records16[ridx]++;
    //     }

    // };
    std::ofstream fout("records34.txt",std::ios_base::out);
    
    for(size_t idx = 0; idx < test_size; idx++){
       for(size_t part = 0; part <task_cnt ; part++){
            threads_vec[part] = std::async(sege,idx);
       }
        for(size_t part = 0; part <task_cnt ; part++){
            threads_vec[part].get();
        }
        std::cout << records[idx] << std::endl;
    }



    for(auto x : records){
        fout << x << std::endl;
        std::cout << x << std::endl;
    }


    fout.close();



    // fout.close();
    // std::ofstream fout24("records24.txt",std::ios_base::out);
    
    // for(size_t idx = 0; idx < test_size; idx++){
    //     for(size_t part = 0; part <128 ; part++){
    //         threads_vec.push_back(tpm.thread_pool().enqueue(sege24,idx));
    //     }
    
    //     for(auto& x :threads_vec){
    //         x.get();
    //     }
    //     std::cout << records24[idx] << std::endl;
        
    //     fout24 <<idx<< "  "<< records24[idx] << std::endl;
    // }

    // fout24.close();


    // std::ofstream fout16("records16.txt",std::ios_base::out);
    
    // for(size_t idx = 0; idx < test_size; idx++){
    //     for(size_t part = 0; part <128 ; part++){
    //         threads_vec.push_back(tpm.thread_pool().enqueue(sege16,idx));
    //     }
    
    //     for(auto& x :threads_vec){
    //         x.get();
    //     }
    //     std::cout << records16[idx] << std::endl;
        
    //     fout16 <<idx<< "  "<< records16[idx] << std::endl;
    // }

    // fout16.close();


}