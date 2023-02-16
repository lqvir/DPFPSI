#include "dhoprf_receiver.h"

// OpenSSL
#include "openssl/rand.h"


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
            EC_GROUP_get_curve_GFp(curve, curve_params_p, curve_params_a, curve_params_b, ctx_b) ;
            BN_BYTE_LEN = BN_num_bits(curve_params_p)/8 + BN_num_bits(curve_params_p)%8;
            POINT_BYTE_LEN = BN_BYTE_LEN * 2 + 1; 
            POINT_COMPRESSED_BYTE_LEN = BN_BYTE_LEN + 1; 

            // uint8_t *pub;
            // point_hex_len = EC_POINT_point2buf(curve, EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,&pub,ctx_b);
            point_hex_len = 33;
            return 0;
        }

        OPRFReceiver::~OPRFReceiver(){
            EC_GROUP_free(curve);
            BN_CTX_free(ctx_b);
            BN_CTX_free(ctx_ecc);
        }

        std::vector<std::string> OPRFReceiver::process_items(gsl::span<const Item> oprf_items){
            size_t item_number = oprf_items.size();
            BIGNUM* oprf_key = BN_new();      
            BIGNUM* item_bn = BN_new();
            EC_POINT* temp = EC_POINT_new(curve);
            std::vector<std::string> out(item_number);
            key_inv.resize(item_number);
            char* outstr;


            for(size_t idx = 0; idx < item_number; idx++){
                auto ref = oprf_items[idx].get_as<char>();
                key_inv[idx] = BN_new();
                temp = BlockToECPoint(curve,ref.data(),NULL);
                
                MakeRandomNonzeroScalar(oprf_key,order);
                EC_POINT_mul(curve,temp,NULL,temp,oprf_key,ctx_ecc);
                BN_mod_inverse(key_inv[idx], oprf_key, order, ctx_ecc);
                
             
                
                unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
                auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
                out[idx] = std::string(buffer,buffer+POINT_COMPRESSED_BYTE_LEN);
                // util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
                // EC_POINT* self_check = EC_POINT_new(curve);
                // EC_POINT_hex2point(curve,out[idx].data(),self_check,ctx_b);
                // std::cout << EC_POINT_cmp(curve,self_check,temp,ctx_b)<< std::endl;
            }
        
            BN_free(oprf_key);
            BN_free(item_bn);
            return out;
        }

        std::vector<OPRFValue> OPRFReceiver::process_response(const std::vector<std::string> responses){
            size_t responses_number = responses.size();
            std::vector<OPRFValue> out(responses_number);
            EC_POINT* temp = EC_POINT_new(curve);
            for(size_t idx = 0; idx < responses_number; idx ++){
                auto &x = responses[idx]; 
                EC_POINT_oct2point(curve,temp,(uint8_t*)responses.at(idx).data(),POINT_COMPRESSED_BYTE_LEN,ctx_b);

                EC_POINT_mul(curve,temp,NULL,temp,key_inv[idx],ctx_ecc);
                unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
                auto len = EC_POINT_point2oct(curve,temp,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,ctx_ecc);
                // std::cout<<len<<std::endl;
                // printf("\n");
                // util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
                util::blake2b512(buffer,POINT_COMPRESSED_BYTE_LEN,out[idx].data(),oprf_value_bytes);
            }
            return out;


        }

        
        
        

    }
}