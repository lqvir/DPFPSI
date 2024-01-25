#pragma once


#include "psi/dpf/dpf_client.h"
#include "psi/dpf/dpf_server.h"
#include "psi/dpf/pcGGM.h"
void test_aes(){
    PSI::Item in(0x0102030405060708,0);
    PSI::Item out;
    PSI::DPF::Keyed_hash_func(in,out);
    uint8_t out1[16];
    PSI::DPF::Keyed_hash_func(in.get_as<uint8_t>().data(),out1);
    PSI::util::printchar(out.get_as<uint8_t>().data(),out.get_as<uint8_t>().size());
    PSI::util::printchar(out1,16);

}



