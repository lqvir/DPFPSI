#include<iostream>
#include<sstream>
#include <cstring>
#include "openssl/aes.h"
#include "openssl/ec.h"
#include "openssl/rand.h"
#include "openssl/obj_mac.h"
#include "openssl/evp.h"
#include <bitset>
#include <bitstream.h>

using namespace std;
void test_openssl(){
    EC_GROUP* curve = EC_GROUP_new_by_curve_name(415);
    EC_POINT* item = EC_POINT_new(curve);
    EC_POINT* value = EC_POINT_new(curve);
    BN_CTX* bn_ctx = BN_CTX_new();

    BIGNUM* key = BN_new();
    uint8_t key_array[16]={0};
    RAND_seed(key_array,16);

    RAND_bytes(key_array,16);
    for(auto x:key_array){
        printf("%02x",x);
    }
    printf("\n");
    // BN_bin2bn(key_array,16,key);
    BN_zero(key);
    EC_POINT_bn2point(curve,key,item,bn_ctx);
    // EC_POINT_mul(curve,value,NULL,item,key,bn_ctx);
    uint8_t *pub;
    // auto public_key_hex_size =  EC_POINT_point2buf(curve, EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,&pub,bn_ctx);

    // cout << public_key_hex_size << endl;
    std::stringstream ss; 
    ss << EC_POINT_point2hex(curve,EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,bn_ctx);
    cout << ss.str() << endl;
    cout << ss.str().size() << endl;
}

void test_bla2k(){
    char a[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10};
    uint8_t md[EVP_MAX_MD_SIZE];
    uint8_t md1[EVP_MAX_MD_SIZE];

    uint32_t md_len = 16;
    EVP_MD_CTX* ctx=EVP_MD_CTX_new();
    EVP_Digest(a,16,md1,&md_len,EVP_blake2b512(),NULL);
    for(auto x:md1){
        printf("%02x",x);
    }
    printf("\n");

    EVP_DigestInit_ex(ctx,EVP_blake2b512(),NULL);
    EVP_DigestUpdate(ctx,a,16);
    EVP_DigestFinal_ex(ctx,md,&md_len);
    for(auto x:md){
        printf("%02x",x);
    }
    cout<<md_len<<endl;
}

void BN_test(){
    EC_GROUP* curve = EC_GROUP_new_by_curve_name(415);
    EC_POINT* item = EC_POINT_new(curve);
    EC_POINT* value = EC_POINT_new(curve);
    BIGNUM* key = BN_new();
    BN_CTX* bn_ctx = BN_CTX_new();
            // point_hex_len = EC_POINT_point2buf(curve, EC_GROUP_get0_generator(curve),POINT_CONVERSION_COMPRESSED,&pub,ctx_b);
    BIGNUM*   curve_params_p = BN_new(); 
    BIGNUM* curve_params_a = BN_new();
    BIGNUM* curve_params_b = BN_new(); 
    EC_GROUP_get_curve_GFp(curve, curve_params_p, curve_params_a, curve_params_b, bn_ctx) ;
    auto BN_BYTE_LEN = BN_num_bits(curve_params_p)/8 + BN_num_bits(curve_params_p)%8;
    auto POINT_BYTE_LEN = BN_BYTE_LEN * 2 + 1; 
    auto POINT_COMPRESSED_BYTE_LEN = BN_BYTE_LEN + 1; 

    // BN_bin2bn(key_array,16,key);
    BN_rand_range(key,EC_GROUP_get0_order(curve));
    // EC_POINT_bn2point(curve,key,item,bn_ctx);
    EC_POINT_mul(curve,item,key,NULL,NULL,NULL);
    cout << EC_POINT_is_on_curve(curve,item,bn_ctx) << endl;
    // item = (EC_POINT*)EC_GROUP_get0_generator(curve);
    unsigned char* buffer = new unsigned char[POINT_COMPRESSED_BYTE_LEN];
    auto l = EC_POINT_point2oct(curve,item,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,bn_ctx);
    cout << l << endl;
    for(size_t i = 0; i < POINT_COMPRESSED_BYTE_LEN; i++){
        printf("%02x",buffer[i]);
    }
    cout << endl;
    EC_POINT_oct2point(curve,value,buffer,POINT_COMPRESSED_BYTE_LEN,bn_ctx);
    cout << EC_POINT_cmp(curve,item,value,bn_ctx) << endl;
}
void test_bitset(){
    uint64_t r[2];
    RAND_bytes((uint8_t*)r,16);
    bitset<128> temp;
    std::stringstream ss;
    ss << bitset<64>(r[0]) << bitset<64>(r[1]);
    ss >> temp;
    // char a[130];
    // _ui64toa(r[0],a,2);
    // cout << string(a) << endl;

    // _ui64toa(r[1],a,2);
    // cout << string(a) << endl;
    // // temp = std::bitset<128>(0);
    // bitset<64> str1(temp.to_string(),64);
    
    cout << temp << endl;
    auto test = bitset<64>(r[0]);
    auto test1 = test.to_ullong() >> 1;
    auto test2 = (test>>1).to_ullong();
    cout << test << endl;
    cout << (test>>1) << endl;
    cout << test[0] << test[1] << endl;

    cout << test1 << endl;
    cout << test2 << endl;
    cout << r[0] << endl;

    uint8_t out1 = 1,out2 = 0;
    uint8 t1 = (out1-out2),t2 = (out2-out1);
    uint8_t out = t1+t2;
    printf("%02x\n",out);
    
}
void test_negative(){
    uint8_t temp = -1;
    uint8_t ans = temp*temp;
    printf("%02x\n",(uint8_t)ans);
}
int main(){
    // test_openssl();
    // test_bitset();
    test_negative();
    cout<<"hello world\n";
    return 0;
}