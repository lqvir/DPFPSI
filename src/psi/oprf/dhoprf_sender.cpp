#include "dhoprf_sender.h"
#include "psi/common/thread_pool_mgr.h"
void test_func(){
    std::cout<<"hello world"<<std::endl;
    return ;
}

namespace PSI{
    namespace OPRF{
        int OPRFSender::init(){
            curve = EC_GROUP_new_by_curve_name(curve_id);
            ctx_b = BN_CTX_new();
            oprf_key = BN_new();
            uint8_t key_array[oprf_key_bytes];
            // RAND_bytes(key_array,oprf_key_bytes);
            // BN_bin2bn(key_array,oprf_key_bytes,oprf_key);

            // non zero
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
            POINT_COMPRESSED_BYTE_LEN = BN_BYTE_LEN + 1; 
            point_hex_len = 33;
            return 0;
        }

        OPRFSender::~OPRFSender(){
            EC_GROUP_free(curve);
            BN_CTX_free(ctx_b);
            BN_free(oprf_key);

        }
        
        OPRFValue  OPRFSender::ComputeItemHash(const Item &item,BN_CTX* ctx){
            
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
            OPRFValue result;
            result.resize(oprf_value_bytes);
            
            util::blake2b512(buffer,POINT_COMPRESSED_BYTE_LEN,result.data(),oprf_value_bytes);
            delete [] buffer;
            return result; 
        }

        std::vector<OPRFValue> OPRFSender::ComputeHashes(const std::span<const Item> &oprf_items){
            ThreadPoolMgr tpm;
            std::vector<OPRFValue> oprf_hashes(oprf_items.size());
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
        std::vector<std::string> OPRFSender::ProcessQeries(const std::vector<std::string> &quries){
            auto quries_number = quries.size();
            std::vector<std::string> out;
            EC_POINT* temp = EC_POINT_new(curve);
            for(auto idx = 0 ; idx < quries_number ; idx ++){
                EC_POINT_oct2point(curve,temp,(uint8_t*)quries.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx_b);
                EC_POINT_mul(curve,temp,NULL,temp,oprf_key,ctx_b);

                unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
                auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx_b);
                // std::cout<<len<<std::endl;
                out.emplace_back(buffer,buffer+POINT_COMPRESSED_BYTE_LEN);

                // EC_POINT* selfcheck = EC_POINT_new(curve);
                // EC_POINT_hex2point(curve,out[idx].data(),selfcheck,ctx_b);
                // std::cout << EC_POINT_cmp(curve,selfcheck,temp,ctx_b) << std::endl;
            }
            return out;
        }
        
  

    } // namespace dhoprf

} // namespace PSI