#include "dhoprf_receiver.h"

// OpenSSL
#include "openssl/rand.h"

#include "psi/common/thread_pool_mgr.h"

#include <memory>

namespace PSI{
    namespace OPRF{

        int OPRFReceiver::init(){
            curve = EC_GROUP_new_by_curve_name(curve_id);
            ctx_b = BN_CTX_new();
            ctx_ecc = BN_CTX_new();
            order = BN_new();


            order = EC_GROUP_get0_order(curve);

            BIGNUM*   curve_params_p = BN_new(); 
            BIGNUM* curve_params_a = BN_new();
            BIGNUM* curve_params_b = BN_new(); 

            EC_GROUP_get_curve(curve, curve_params_p, curve_params_a, curve_params_b, ctx_b) ;
            BN_BYTE_LEN = BN_num_bits(curve_params_p)/8 + BN_num_bits(curve_params_p)%8;
            POINT_BYTE_LEN = BN_BYTE_LEN * 2 + 1; 
            assert((BN_BYTE_LEN+1) == POINT_COMPRESSED_BYTE_LEN);
            
            // uint8_t *pub;
            // point_hex_len = EC_POINT_point2buf(curve, EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,&pub,ctx_b);
            return 0;
        }

        OPRFReceiver::~OPRFReceiver(){
            EC_GROUP_free(curve);
            BN_CTX_free(ctx_b);
            BN_CTX_free(ctx_ecc);
        }

        std::vector<OPRFPointOpenSSL> OPRFReceiver::process_items(std::span<const Item> oprf_items){
            size_t item_number = oprf_items.size();
            BIGNUM* oprf_key = BN_new();      
            BIGNUM* item_bn = BN_new();
            EC_POINT* ItemPoint = EC_POINT_new(curve);
            std::vector<OPRFPointOpenSSL> out(item_number);
            key_inv.resize(item_number);


            for(size_t idx = 0; idx < item_number; idx++){
                auto ref = oprf_items[idx].get_as<char>();
                key_inv[idx] = BN_new();
                ItemPoint = BlockToECPoint(curve,ref.data(),NULL);
                
            // {
            //     unsigned char* buffer1 = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
            //     auto ll = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer1,POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
            //     printf("re");
            //     util::printchar(buffer1,POINT_COMPRESSED_BYTE_LEN);
            // }

                MakeRandomNonzeroScalar(oprf_key,order);
                EC_POINT_mul(curve,ItemPoint,NULL,ItemPoint,oprf_key,ctx_ecc);
                BN_mod_inverse(key_inv[idx], oprf_key, order, ctx_ecc);
                
             
                
                auto len = EC_POINT_point2oct(curve,ItemPoint,POINT_CONVERSION_COMPRESSED,out[idx].data(),POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
                // util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
                // EC_POINT* self_check = EC_POINT_new(curve);
                // EC_POINT_hex2point(curve,out[idx].data(),self_check,ctx_b);
                // std::cout << EC_POINT_cmp(curve,self_check,temp,ctx_b)<< std::endl;
            }
        
            BN_free(oprf_key);
            BN_free(item_bn);
            return out;
        }
        std::vector<OPRFPointOpenSSL> OPRFReceiver::process_items_threads(std::span<const Item> oprf_items){
            ThreadPoolMgr tpm;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), oprf_items.size());
            size_t item_number = oprf_items.size();
            
            std::vector<std::future<void>> futures(task_count);
            std::vector<OPRFPointOpenSSL> out(item_number);
            key_inv.resize(item_number);

            auto ProcessItemLambda = [&](size_t start_idx,size_t step) {
                BN_CTX* ctx = BN_CTX_new();
                BIGNUM* oprf_key = BN_new();      

                for(size_t idx = start_idx; idx < item_number; idx+= step){
                    auto ref = oprf_items[idx].get_as<char>();
                    key_inv[idx] = BN_new();
                    auto temp = BlockToECPoint(curve,ref.data(),ctx);

                    MakeRandomNonzeroScalar(oprf_key,order);
                    EC_POINT_mul(curve,temp,NULL,temp,oprf_key,ctx);
                    BN_mod_inverse(key_inv[idx], oprf_key, order, ctx);
                    auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,out[idx].data(),POINT_COMPRESSED_BYTE_LEN,ctx);
                }
                BN_free(oprf_key);
                BN_CTX_free(ctx);
            };

            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessItemLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return out;

        }
        std::vector<OPRFValueOpenssL> OPRFReceiver::process_response(const std::vector<OPRFPointOpenSSL>& responses){
            size_t responses_number = responses.size();
            std::vector<OPRFValueOpenssL> out(responses_number);
            EC_POINT* temp = EC_POINT_new(curve);
            for(size_t idx = 0; idx < responses_number; idx ++){
                auto &x = responses[idx]; 
                EC_POINT_oct2point(curve,temp,(uint8_t*)responses.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx_b);

                EC_POINT_mul(curve,temp,NULL,temp,key_inv[idx],ctx_ecc);
                unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
                auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
      
                util::blake2b512(buffer,POINT_COMPRESSED_BYTE_LEN,out[idx].data(),oprf_value_bytes);
            }
            EC_POINT_free(temp);

            return out;


        }
        std::vector<OPRFValueOpenssL> OPRFReceiver::process_response_threads(const std::vector<OPRFPointOpenSSL>& responses){
            ThreadPoolMgr tpm;
            size_t responses_number = responses.size();
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), responses_number);
            std::vector<std::future<void>> futures(task_count);
            
            std::vector<OPRFValueOpenssL> out(responses_number);
            auto ProcessResponseLambda = [&](size_t start_idx,size_t step){
                EC_POINT* temp = EC_POINT_new(curve);
                BN_CTX* ctx = BN_CTX_new();

                for(size_t idx = start_idx; idx < responses_number ;idx += step){
                    auto &x = responses[idx]; 
                    EC_POINT_oct2point(curve,temp,(uint8_t*)responses.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx);

                    EC_POINT_mul(curve,temp,NULL,temp,key_inv[idx],ctx);
                    unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
                    auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx);
                    util::blake2b512(buffer,POINT_COMPRESSED_BYTE_LEN,out[idx].data(),oprf_value_bytes);

                }

                BN_CTX_free(ctx);
                EC_POINT_free(temp);
            };
        
            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessResponseLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return out;

        }
        
        std::vector<OPRFPointFourQ> 
            OPRFReceiver::process_itemsFourQ(std::span<const Item> oprf_items){
            
            size_t item_number = oprf_items.size();
            key_inv_fourq.resize(item_number);
            std::vector<OPRFPointFourQ> out(item_number);
            for(size_t idx = 0; idx < item_number; idx++){

                ECPointFourQ ecpt(oprf_items[idx].get_as<const unsigned char>());

                ECPointFourQ::scalar_type random_scalar;
                ECPointFourQ::MakeRandomNonzeroScalar(random_scalar);
                ECPointFourQ::InvertScalar(random_scalar, key_inv_fourq.at(idx));

                // Multiply our point with the random scalar
                ecpt.scalar_multiply(random_scalar, false);

                // Save the result to items_buffer
                ecpt.save(out[idx]);

            }

            return out;
        }
        std::vector<OPRFPointFourQ> 
            OPRFReceiver::process_items_threadsFourQ(std::span<const Item> oprf_items){
           
            ThreadPoolMgr tpm;
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), oprf_items.size());
            size_t item_number = oprf_items.size();
            std::vector<std::future<void>> futures(task_count);
            std::vector<OPRFPointFourQ> out(item_number);
            key_inv_fourq.resize(item_number);

            auto ProcessItemLambda = [&](size_t start_idx,size_t step) {
       
                for(size_t idx = start_idx; idx < item_number; idx+= step){
                    auto values = oprf_items[idx].get_as<const unsigned char>();
                    // util::printchar((uint8_t*)values.data(),values.size());
                    ECPointFourQ ecpt(values);

                    ECPointFourQ::scalar_type random_scalar;
                    ECPointFourQ::MakeRandomNonzeroScalar(random_scalar);
                    ECPointFourQ::InvertScalar(random_scalar, key_inv_fourq.at(idx));

                    
                    // Multiply our point with the random scalar
                    ecpt.scalar_multiply(random_scalar, false);
                    // ecpt.scalar_multiply(key_inv_fourq.at(idx),false);
                    // Save the result to items_buffer
                    ecpt.save(out[idx]);
                    
              
            // util::printchar(out[idx].data(),out[idx].size());
                }
              
            };
            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessItemLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }
            return out;

        }
        std::vector<OPRFValueOpenssL> 
            OPRFReceiver::process_responseFourQ(const std::vector<OPRFPointFourQ>& responses){
                size_t responses_number = responses.size();

                std::vector<OPRFValueOpenssL> out(responses_number);
                for(size_t idx = 0; idx < responses_number; idx ++){
                    ECPointFourQ ecpt;
                    ecpt.load(responses[idx]);
                    // Multiply with inverse random scalar
                    ecpt.scalar_multiply(key_inv_fourq[idx], false);
                    ecpt.extract_hash(out[idx]);
                }

            return out;

        }
        std::vector<OPRFValueOpenssL> 
            OPRFReceiver::process_response_threadsFourQ(const std::vector<OPRFPointFourQ>& responses){
        // std::cout <<__FILE__<<":" << __LINE__ << std::endl;
            
            ThreadPoolMgr tpm;
            size_t responses_number = responses.size();
            size_t task_count =std::min<size_t>(ThreadPoolMgr::GetThreadCount(), responses_number);
            std::vector<std::future<void>> futures(task_count);
            
            std::vector<OPRFValueOpenssL> out(responses_number);
            auto ProcessResponseLambda = [&](size_t start_idx,size_t step){
                for(size_t idx = start_idx; idx < responses_number ;idx += step){
                    ECPointFourQ ecpt;
                    ecpt.load(responses[idx]);
                    // Multiply with inverse random scalar
                    ecpt.scalar_multiply(key_inv_fourq[idx], false);
                    ecpt.extract_hash(out[idx]);
                }
            };
        
            for (size_t thread_idx = 0; thread_idx < task_count; thread_idx++) {
                futures[thread_idx] =
                    tpm.thread_pool().enqueue(ProcessResponseLambda, thread_idx, task_count);
            }

            for (auto &f : futures) {
                f.get();
            }

            return out;

        }
        

        

    }
}