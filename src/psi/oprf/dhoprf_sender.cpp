#include "dhoprf_sender.h"
#include "psi/common/thread_pool_mgr.h"
void test_func(){
    std::cout<<"hello world"<<std::endl;
    return ;
}

namespace PSI{
    namespace DHOPRF{
        int OPRFSender::init(){
            curve = EC_GROUP_new_by_curve_name(curve_id);
            ctx_b = BN_CTX_new();
            oprf_key = BN_new();
            uint8_t key_array[oprf_key_bytes];
            // RAND_bytes(key_array,oprf_key_bytes);
            // BN_bin2bn(key_array,oprf_key_bytes,oprf_key);
            
            // non zero
            ECPointFourQ::MakeRandomNonzeroScalar(oprf_key_fourq);
            MakeRandomNonzeroScalar(oprf_key,EC_GROUP_get0_order(curve));
            // while(BN_is_zero(oprf_key)){
            //     BN_rand_range(oprf_key,EC_GROUP_get0_order(curve));
            // }
            // uint8_t *pub;
            // point_hex_len = EC_POINT_point2buf(curve, EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,&pub,ctx_b);
            BIGNUM*   curve_params_p = BN_new(); 
            BIGNUM* curve_params_a = BN_new();
            BIGNUM* curve_params_b = BN_new(); 
            EC_GROUP_get_curve(curve, curve_params_p, curve_params_a, curve_params_b, ctx_b) ;
            BN_BYTE_LEN = BN_num_bits(curve_params_p)/8 + BN_num_bits(curve_params_p)%8;
            POINT_BYTE_LEN = BN_BYTE_LEN * 2 + 1; 
            assert((BN_BYTE_LEN+1) == POINT_COMPRESSED_BYTE_LEN);
            point_hex_len = 33;
            return 0;
        }

        OPRFSender::~OPRFSender(){
            EC_GROUP_free(curve);
            BN_CTX_free(ctx_b);
            BN_free(oprf_key);

        }
        
        OPRFValueOpenssL  OPRFSender::ComputeItemHash(const Item &item,BN_CTX* ctx){
            
            // item type change 


            EC_POINT* EC_item = EC_POINT_new(curve);
            EC_POINT* EC_OPRFValue = EC_POINT_new(curve); 
      
            EC_item = BlockToECPoint(curve,item.get_as<char>().data(),ctx);
            // unsigned char* buffer1 = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
            // auto ll = EC_POINT_point2oct(curve,EC_item,POINT_CONVERSION_COMPRESSED,buffer1,POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
            // printf("se");
            // util::printchar(buffer1,POINT_COMPRESSED_BYTE_LEN);

            EC_POINT_mul(curve,EC_OPRFValue,NULL,EC_item,oprf_key,ctx);
            // std::stringstream ss; 
            // ss << EC_POINT_point2hex(curve,EC_OPRFValue,POINT_CONVERSION_COMPRESSED,ctx_b);
            // std::cout << ss.str() << std::endl;
            unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
            
            auto l = EC_POINT_point2oct(curve,EC_OPRFValue,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx);
            // std::cout<<l<<std::endl;
            // util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
            // std::cout << std::endl;
            OPRFValueOpenssL result;
            
            util::blake2b512(buffer,POINT_COMPRESSED_BYTE_LEN,result.data(),oprf_value_bytes);
            delete [] buffer;
            return result; 
        }

        std::vector<OPRFValueOpenssL> OPRFSender::ComputeHashes(const std::span<const Item> &oprf_items){
            ThreadPoolMgr tpm;
            std::vector<OPRFValueOpenssL> oprf_hashes(oprf_items.size());
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), oprf_items.size());
            std::vector<std::future<void>> futures(task_count);

             auto ComputeHashesLambda = [&](size_t start_idx, size_t step) {
                BN_CTX* ctx = BN_CTX_new();
                for (size_t idx = start_idx; idx < oprf_items.size(); idx += step) {

                    oprf_hashes[idx] = ComputeItemHash(oprf_items[idx],ctx);
                }
                BN_CTX_free(ctx);
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ComputeHashesLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return oprf_hashes;
        }
        std::vector<OPRFPoint> OPRFSender::ProcessQueries(const std::vector<OPRFPoint> &queries){
            auto queries_number = queries.size();
            std::vector<OPRFPoint> out;
            EC_POINT* temp = EC_POINT_new(curve);
            for(auto idx = 0 ; idx < queries_number ; idx ++){
                EC_POINT_oct2point(curve,temp,(uint8_t*)queries.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx_b);
                EC_POINT_mul(curve,temp,NULL,temp,oprf_key,ctx_b);

                auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,out[idx].data(),POINT_COMPRESSED_BYTE_LEN,ctx_b);
                // std::cout<<len<<std::endl;

                // EC_POINT* selfcheck = EC_POINT_new(curve);
                // EC_POINT_hex2point(curve,out[idx].data(),selfcheck,ctx_b);
                // std::cout << EC_POINT_cmp(curve,selfcheck,temp,ctx_b) << std::endl;
            }
            EC_POINT_free(temp);

            return out;
        }
        std::vector<OPRFPoint> OPRFSender::ProcessQueriesThread(const std::vector<OPRFPoint> &queries){
            ThreadPoolMgr tpm;
            size_t queries_number = queries.size();
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), queries_number);
            std::vector<std::future<void>> futures(task_count);
            std::vector<OPRFPoint> out(queries_number);

            auto ProcessQueriesLambda = [&](size_t start_idx,size_t step){
                BN_CTX* ctx = BN_CTX_new();
                EC_POINT* temp = EC_POINT_new(curve);
                for(size_t idx = start_idx;idx < queries_number;idx+= step){

                    EC_POINT_oct2point(curve,temp,(uint8_t*)queries.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx);
                    EC_POINT_mul(curve,temp,NULL,temp,oprf_key,ctx);

                    auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,out[idx].data(),POINT_COMPRESSED_BYTE_LEN,ctx);
                    // std::cout<<len<<std::endl;
                    
                }

                EC_POINT_free(temp);
                BN_CTX_free(ctx);
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessQueriesLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }

            return out;
        }
        OPRFValueOpenssL  OPRFSender::ComputeItemHashFourQ(const Item &item){
            ECPointFourQ ecpt(item.get_as<const unsigned char>());

            // Multiply with key
            if(!ecpt.scalar_multiply(oprf_key_fourq, true)){
            }
            OPRFValueOpenssL out;
            // Extract the item hash and the label encryption key
            ecpt.extract_hash(out);
            return out;
        }
        std::vector<OPRFValueOpenssL> OPRFSender::ComputeHashesFourQ(const std::span<const Item> &oprf_items){
            ThreadPoolMgr tpm;
            std::vector<OPRFValueOpenssL> oprf_hashes(oprf_items.size());
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), oprf_items.size());
            std::vector<std::future<void>> futures(task_count);

            auto ComputeHashesLambda = [&](size_t start_idx, size_t step) {
                for (size_t idx = start_idx; idx < oprf_items.size(); idx += step) {
                    oprf_hashes[idx] = ComputeItemHashFourQ(oprf_items[idx]);
                }
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ComputeHashesLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return oprf_hashes;
        }
        std::vector<OPRFPointFourQ> OPRFSender::ProcessQueriesFourQ(std::vector<OPRFPointFourQ> &queries){
            auto queries_number = queries.size();
            std::vector<OPRFPointFourQ> out(queries_number);
            for(auto idx = 0 ; idx < queries_number ; idx ++){
                ECPointFourQ ecpt;
                ecpt.load(queries[idx]);
                ecpt.scalar_multiply(oprf_key_fourq, true);
                ecpt.save(out[idx]);
            }
            return out;
        }
        std::vector<OPRFPointFourQ> OPRFSender::ProcessQueriesThreadFourQ(std::vector<OPRFPointFourQ> &queries){
            auto queries_number = queries.size();
            std::vector<OPRFPointFourQ> out(queries_number);
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), queries_number);
            std::vector<std::future<void>> futures(task_count);
            ThreadPoolMgr tpm;
        
            auto ProcessQueriesLambda = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx;idx < queries_number;idx+= step){
                    ECPointFourQ ecpt;
                    ecpt.load(queries[idx]);

                    if(!ecpt.scalar_multiply(oprf_key_fourq, true)){
                        printf("%d\n",__LINE__);
                    };
                    ecpt.save(out[idx]);

                       // std::cout<<len<<std::endl;
                }
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessQueriesLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return out;
        }
        

    } // namespace dhoprf

} // namespace PSI