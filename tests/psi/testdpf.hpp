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
    PSI::DPF::dpf_client test0;
    PSI::DPF::dpf_client test1;

    test.init();
    test.Gen(15,1);
    auto t1 = test.get_key0();
    auto t2 = test.get_key1();
    test0.init(t1);
    auto x1 = test0.Eval(14,0);
    test1.init(t2);
    auto x2 = test1.Eval(14,1);
    auto ans = test.reconstruct(x1,x2);
    printf("%02x\n",x1);
    printf("%02x\n",x2);

    printf("%02x\n",ans);
    
}