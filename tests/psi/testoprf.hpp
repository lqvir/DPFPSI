#pragma once

#include "psi/oprf/dhoprf_sender.h"
#include "psi/oprf/dhoprf_receiver.h"
#include "psi/common/thread_pool_mgr.h"
#include "psi/common/item.h"

#include "psi/oprf/GCOPRF_receiver.h"
#include "psi/oprf/GCOPRF_sender.h"
#include "psi/common/stopwatch.h"
#include <chrono>
#include <vector>
using namespace PSI::DHOPRF;

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
    std::span<PSI::Item> input(input_V.data(),10);
    auto value = oprfsender.ComputeHashes(input);
    for(auto x: value){
        PSI::util::printchar((unsigned char*) x.data(),PSI::OPRFValueBytes);
    }
}
void testReceiver(){
    OPRFReceiver oprfreceier;
    oprfreceier.init();

    std::vector<PSI::Item> input_V;
    for(int i = 0; i < 10; i++){
        input_V.emplace_back(i*1315158,i<<20);
    }
    std::span<PSI::Item> input(input_V.data(),10);
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

    for(size_t idx = 0; idx < 1024 ; idx ++){
        ReceiverSet.emplace_back(0x123+idx,0x456*idx);
        ServerSet.emplace_back(0x123+idx,0x456*idx);   
    }
    
    for(size_t idx = 0; idx < 1048576 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }

    PSI::StopWatch oprftimer("oprftimer");
    oprftimer.setDurationStart("server start");
    auto value = oprfsender.ComputeHashes(ServerSet);
    oprftimer.setDurationEnd("server start");
    oprftimer.setDurationStart("client start");
    
    auto qurie = oprfreceier.process_items(ReceiverSet);
    auto query = oprfreceier.process_items_threads(ReceiverSet);
    
    // for(size_t idx = 0; idx < query.size(); idx++){
    //     assert(query[idx] == qurie[idx]);

    // }
    
    auto response = oprfsender.ProcessQueriesThread(query);
    auto rvalue = oprfreceier.process_response_threads(response);
    oprftimer.setDurationEnd("client start");
    
    // for(auto x:value){
    //      PSI::util::printchar((unsigned char*) x.data(),PSI::OPRFValueBytes);
    // }
    // std::cout << std::endl;
    // for(auto x:rvalue){
    //      PSI::util::printchar((unsigned char*) x.data(),PSI::OPRFValueBytes);
    // }
    oprftimer.printDurationRecord();
}
void test_OPRF_FourQ(){
    OPRFSender oprfsender;
    OPRFReceiver oprfreceier;

    oprfsender.init();
    oprfreceier.init();
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    for(size_t idx = 0; idx < 16 ; idx ++){
        ReceiverSet.emplace_back(0x123,0x456);
        ServerSet.emplace_back(0x123,0x456);   
    }
    
    for(size_t idx = 0; idx < 1048576 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }
    PSI::StopWatch oprftimer("oprftimer");
    oprftimer.setDurationStart("server start");
    auto value = oprfsender.ComputeHashesFourQ(ServerSet);
        oprftimer.setDurationEnd("server start");
    oprftimer.setDurationStart("client start");
    // auto qurie = oprfreceier.process_itemsFourQ(ReceiverSet);

    auto query = oprfreceier.process_items_threadsFourQ(ReceiverSet);

    // std::cout << __LINE__ << std::endl;
    // PSI::util::printchar(query[0].data(),query[0].size());
    // for(size_t idx = 0; idx < query.size(); idx++){
    //     assert(query[idx] == qurie[idx]);
    // }
    
    auto response = oprfsender.ProcessQueriesThreadFourQ(query);
    // std::cout << __LINE__ << std::endl;
    // for(auto x: response){
    //     PSI::util::printchar(x.data(),x.size());
    // }
    auto rvalue = oprfreceier.process_response_threadsFourQ(response);
    oprftimer.setDurationEnd("client start");
    oprftimer.printDurationRecord();

    // for(auto x:value){
    //      PSI::util::printchar((unsigned char*) x.data(),PSI::OPRFValueBytes);
    // }
    // std::cout << std::endl;
    // for(auto x:rvalue){
    //      PSI::util::printchar((unsigned char*) x.data(),PSI::OPRFValueBytes);
    // }


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
        
        auto res = PSI::DHOPRF::BlockToECPoint(curve,x.get_as<char>().data(),NULL);
        uint8_t buffer1[33];
        auto ll = EC_POINT_point2oct(curve,res,POINT_CONVERSION_COMPRESSED,buffer1,33,bn_ctx);
        printf("se");
        PSI::util::printchar(buffer1,33);
    }
    for(auto x:ReceiverSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);

        auto res = PSI::DHOPRF::BlockToECPoint(curve,x.get_as<char>().data(),NULL);
        uint8_t buffer1[33];
        auto ll = EC_POINT_point2oct(curve,res,POINT_CONVERSION_COMPRESSED,buffer1,33,bn_ctx);
        printf("se");
        PSI::util::printchar(buffer1,33);

    }
}

void test_fourQ(){
    PSI::Item cont(0,1);
    ECPointFourQ ecpt(cont.get_as<uint8_t>());
    std::array<uint8_t,32> out;
    ecpt.save(out);
    PSI::util::printchar(out.data(),out.size());
    ECPointFourQ ecpt1;
    ecpt1.load(out);
    ECPointFourQ::scalar_type random_scalar;
    ECPointFourQ::scalar_type oprf_key;
    ECPointFourQ::MakeRandomNonzeroScalar(random_scalar);
    ECPointFourQ::MakeRandomNonzeroScalar(oprf_key);

    ECPointFourQ::scalar_type random_scalar_inv;
    ECPointFourQ::InvertScalar(random_scalar,random_scalar_inv);

    bool ans = ecpt1.scalar_multiply(random_scalar, false);
    if(!ans){
        printf("??");
    }

    ans = ecpt1.scalar_multiply(oprf_key, true);
    if(!ans){
        printf("??");
    }

    ans = ecpt1.scalar_multiply(random_scalar_inv, false);
    if(!ans){
        printf("??");
    }
    ecpt1.save(out);
    PSI::util::printchar(out.data(),out.size());
}


void test_lowerMC(){

    auto recevierinput = std::make_unique<std::vector<droidCrypto::block>>();
    auto senderinput = std::make_unique<std::vector<droidCrypto::block>>();
    droidCrypto::SecureRandom rnd;
    for(size_t idx = 0; idx < 4; idx++){

        recevierinput->emplace_back(droidCrypto::AllOneBlock);
        recevierinput->emplace_back(rnd.randBlock());
        senderinput->emplace_back(droidCrypto::AllOneBlock);
        senderinput->emplace_back(rnd.randBlock());
        senderinput->emplace_back(rnd.randBlock());
        senderinput->emplace_back(rnd.randBlock());
    }

    auto routine = [&]{

        droidCrypto::CSocketChannel chanc("127.0.0.1", 8000, false);
        printf("%d\n",__LINE__);
        PSI::GCOPRF::OPRFReciver r(chanc);

        r.base(8);
        auto x = r.Online(std::move(recevierinput));
        PSI::util::printchar((*x)[0].data(),PSI::OPRFValueBytes);
    };
    auto threads = std::async(routine);
        printf("%d\n",__LINE__);

    droidCrypto::CSocketChannel chans("127.0.0.1", 8000, true);
    PSI::GCOPRF::OPRFSender s(chans);
    auto sout = s.setup(std::move(senderinput));
    PSI::util::printchar((*sout)[0].data(),PSI::OPRFValueBytes);

    s.base();
    s.Online();
    threads.get();
  

}