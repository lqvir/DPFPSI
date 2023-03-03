#pragma once
#include "gsl/span"
#include "openssl/ec.h"
#include "openssl/obj_mac.h"
#include <cstring>
#include <vector>
#include "openssl/evp.h"
#include "openssl/sha.h"

#include "psi/param.h"
namespace PSI{
    namespace OPRF{
        constexpr size_t oprf_key_size = 128;
        constexpr size_t oprf_key_bytes = 16;
        constexpr size_t oprf_value_bytes = oprf_key_bytes + leading_zero_length + Label_byte_size;
        constexpr  int curve_id = NID_X9_62_prime256v1 ;
        #define BasicHash(input, HASH_INPUT_LEN, output) SHA256(input, HASH_INPUT_LEN, output)
       
        typedef std::string OPRFValue;

        inline void MakeRandomNonzeroScalar(BIGNUM* out,const BIGNUM* range){
            BN_rand_range(out,range);
            while(BN_is_zero(out)){
                BN_rand_range(out,range);
            }
        }
        inline EC_POINT* BlockToECPoint(EC_GROUP* curve,const char* var,BN_CTX* ctx)
        {
            EC_POINT* ecp_result = EC_POINT_new(curve); 
        
            BIGNUM *x = BN_new();
            unsigned char buffer[32];
            memset(buffer, 0, 32);
            memcpy(buffer, &var, 16);  // load block to buffer  
            BasicHash(buffer, 16, buffer); // initial hash to get the indication bit of y coordinate
            int y_bit = 0x01 & buffer[0]; // this is an ad-hoc method
            while (true) { 
                BasicHash(buffer, 32, buffer); // iterated hash, modeled as random oracle
                BN_bin2bn(buffer, 32, x);
                if(EC_POINT_set_compressed_coordinates(curve, ecp_result, x, y_bit, ctx)==1) break;              
            }
            BN_free(x);    
            return ecp_result;
        }

    }
}