#pragma once

#include "psi/oprf/dhoprf_sender.h"
#include "psi/oprf/dhoprf_receiver.h"
#include "psi/common/thread_pool_mgr.h"
#include "psi/common/item.h"
#include <chrono>
#include "gsl/span"
#include <vector>
using namespace PSI::OPRF;

void func(int i){
}

void test_thread_pool(){
    auto routine = [&](int i){
        std::cout<<i<<std::endl;

    };

    std::vector<std::future<void>> futures;

    PSI::ThreadPoolMgr tpm;
    for(int i = 0; i <10; i++){
        futures.push_back(tpm.thread_pool().enqueue(routine,i));
    }

    for(int i = 0; i < 10; i++){
        futures[i].get();
    }
}
void test_hashitem(){
    OPRFSender oprfsender;
    oprfsender.init();
    PSI::Item item(1,2);
    auto value = oprfsender.ComputeItemHash(item);
    for(auto x:value){
        printf("%02X",(unsigned char)x);
    }
    printf("\n");
}
void test_hashitems(){
    OPRFSender oprfsender;
    oprfsender.init();

    std::vector<PSI::Item> input_V;
    for(int i = 0; i < 10; i++){
        input_V.emplace_back(i*1315158,i<<20);
    }
    gsl::span<PSI::Item> input(input_V.data(),10);
    auto value = oprfsender.ComputeHashes(input);
    for(auto x: value){
        PSI::util::printchar((unsigned char*) x.data(),oprf_value_bytes);
    }
}
void testReceiver(){
    OPRFReceiver oprfreceier;
    oprfreceier.init();

    std::vector<PSI::Item> input_V;
    for(int i = 0; i < 10; i++){
        input_V.emplace_back(i*1315158,i<<20);
    }
    gsl::span<PSI::Item> input(input_V.data(),10);
    oprfreceier.process_items(input);
}
void testoprf(){
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
    char input[16];
    RAND_bytes((uint8_t*)input,16);
    
    EC_POINT* base = BlockToECPoint(curve,input,bn_ctx);
    for(int i = 0; i < 10 ; i++){
        uint8_t* buffer = new uint8_t[POINT_COMPRESSED_BYTE_LEN];
        auto len =  EC_POINT_point2oct(curve,base,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,bn_ctx);
        std::cout << len << std::endl;
        PSI::util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
    }


    BIGNUM* bn = BN_new();
    BIGNUM* mn = BN_new();

    BIGNUM* inv = BN_new();
    const BIGNUM* order = EC_GROUP_get0_order(curve);
    BN_rand_range(bn,order);
    BN_rand_range(mn,order);

    BN_mod_inverse(inv,bn,order,bn_ctx);
    EC_POINT* ans = EC_POINT_new(curve);
    EC_POINT_mul(curve,ans,NULL,base,bn,bn_ctx);
    EC_POINT_mul(curve,ans,NULL,ans,mn,bn_ctx);
    EC_POINT_mul(curve,ans,NULL,ans,inv,bn_ctx);

    EC_POINT_mul(curve,value,NULL,base,mn,bn_ctx);
    uint8_t* buffer = new uint8_t[POINT_COMPRESSED_BYTE_LEN];
    auto len =  EC_POINT_point2oct(curve,ans,POINT_CONVERSION_COMPRESSED,buffer,POINT_COMPRESSED_BYTE_LEN,bn_ctx);
    std::cout << len << std::endl;
    PSI::util::printchar(buffer,POINT_COMPRESSED_BYTE_LEN);
    std::cout << EC_POINT_cmp(curve,value,ans,bn_ctx) << std::endl;

}
void testOPRF(){
    OPRFSender oprfsender;
    OPRFReceiver oprfreceier;

    oprfsender.init();
    oprfreceier.init();
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    for(size_t idx = 0; idx < 16 ; idx ++){
        ReceiverSet.emplace_back(0x123+idx,0x456*idx);
        ServerSet.emplace_back(0x123+idx,0x456*idx);   
    }
    
    for(size_t idx = 0; idx < 1024 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }
    auto value = oprfsender.ComputeHashes(ServerSet);
    
    auto qurie = oprfreceier.process_items(ReceiverSet);
    auto response = oprfsender.ProcessQeries(qurie);
    auto rvalue = oprfreceier.process_response(response);
    for(auto x:value){
         PSI::util::printchar((unsigned char*) x.data(),oprf_value_bytes);
    }
    std::cout << std::endl;
    for(auto x:rvalue){
         PSI::util::printchar((unsigned char*) x.data(),oprf_value_bytes);
    }
}


void test_to_ECPOINT(){
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    for(size_t idx = 0; idx < 4 ; idx ++){
        uint64_t buff[2];
        RAND_bytes((uint8_t*)buff,16);
        ReceiverSet.emplace_back(buff[0],buff[1]);
        ServerSet.emplace_back(buff[0],buff[1]);   
    }
    
    for(size_t idx = 0; idx < 12 ; idx ++){
        uint64_t buff[2];
        RAND_bytes((uint8_t*)buff,16);
        ServerSet.emplace_back(buff[0],buff[1]);   

    }
    BN_CTX* bn_ctx = BN_CTX_new();
    EC_GROUP* curve = EC_GROUP_new_by_curve_name(415);
    for(auto x:ServerSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);
        
        auto res = PSI::OPRF::BlockToECPoint(curve,x.get_as<char>().data(),NULL);
        uint8_t buffer1[33];
        auto ll = EC_POINT_point2oct(curve,res,POINT_CONVERSION_COMPRESSED,buffer1,33,bn_ctx);
        printf("se");
        PSI::util::printchar(buffer1,33);
    }
    for(auto x:ReceiverSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);

        auto res = PSI::OPRF::BlockToECPoint(curve,x.get_as<char>().data(),NULL);
        uint8_t buffer1[33];
        auto ll = EC_POINT_point2oct(curve,res,POINT_CONVERSION_COMPRESSED,buffer1,33,bn_ctx);
        printf("se");
        PSI::util::printchar(buffer1,33);

    }
}