#pragma once


#include "psi/dpf/dpf_client.h"
#include "psi/dpf/dpf_server.h"

void test_aes(){
    PSI::Item in(0x0102030405060708,0);
    PSI::Item out;
    PSI::DPF::Keyed_hash_func(in,out);
    uint8_t out1[16];
    PSI::DPF::Keyed_hash_func(in.get_as<uint8_t>().data(),out1);
    PSI::util::printchar(out.get_as<uint8_t>().data(),out.get_as<uint8_t>().size());
    PSI::util::printchar(out1,16);

}

void test_dpf(){
    PSI::DPF::dpf_server test;
    PSI::DPF::dpf_client test0(0);
    PSI::DPF::dpf_client test1(1);

    test.init();
    PSI::DPF::DPFKey t1,t2;
    test.Gen(15,1,t1,t2);

    
    auto x1 = test0.Eval(14,t1);
    auto x2 = test1.Eval(14,t2);
    auto ans = test.reconstruct(x1,x2);
    printf("%02x\n",x1);
    printf("%02x\n",x2);

    printf("%02x\n",ans);
    
}

void test_GenTree(){
    PSI::DPF::dpf_server test;
    PSI::DPF::dpf_client server(0);
    PSI::DPF::dpf_client aid(1);
    test.init();
    PSI::DPF::DPFKey key0,key1;
    test.Gen(23,1,key0,key1);

    auto tree0 = server.DPFGenTree(key0);
    auto tree1 = aid.DPFGenTree(key1);
    for(size_t idx = 0; idx < PSI::cuckoo::block_size;idx++){
        auto x1 = server.Eval(idx,key0);
        auto x2 = aid.Eval(idx,key1);
        printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,x1,x2,test.reconstruct(x1,x2));
        printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,tree0[idx],tree1[idx],test.reconstruct(tree0[idx],tree1[idx]));

    }
}

