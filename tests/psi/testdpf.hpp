#pragma once


#include "psi/dpf/dpf_client.h"
#include "psi/dpf/dpf_server.h"
#include "psi/dpf/pcGGM.h"

#include "psi/common/BitOpreate.h"

void test_dpf(){

    std::chrono::duration<double> buildTb, evalTb, answerTb;

    PSI::DPF::pcGGM::DPFKey ka,kb;
        auto time1b = std::chrono::high_resolution_clock::now();

    PSI::DPF::pcGGM::GenKey(201,1,ka,kb);

        auto time2b = std::chrono::high_resolution_clock::now();
    uint64_t ansa[PSI::DPF::DPF_OUTPUT_U64COUNT];
    uint64_t ansb[PSI::DPF::DPF_OUTPUT_U64COUNT];
    uint64_t ansc[PSI::DPF::DPF_OUTPUT_U64COUNT];
    uint64_t ansd[PSI::DPF::DPF_OUTPUT_U64COUNT];

    PSI::DPF::pcGGM::GenTreeRecursion(ka,ansa);

    auto time3b = std::chrono::high_resolution_clock::now();

    PSI::DPF::pcGGM::GenTreeRecursion(kb,ansb);
    PSI::DPF::pcGGM::DPFKey kc,kd;
    std::chrono::duration<double> buildT, evalT, answerT;


    auto time1 = std::chrono::high_resolution_clock::now();
    PSI::DPF::pcGGM::GenKey(200,1,kc,kd);

    auto time2 = std::chrono::high_resolution_clock::now();

     PSI::DPF::pcGGM::GenTree(kc,ansc);

    auto time3 = std::chrono::high_resolution_clock::now();
    PSI::DPF::pcGGM::GenTree(kd,ansd);

    auto anse = PSI::DPF::pcGGM::GenTree(kc);
    auto ansf = PSI::DPF::pcGGM::GenTree(kd);
         buildTb = time2b - time1b;
         evalTb = time3b - time2b;

         buildT = time2 - time1;
         evalT = time3 - time2;


    PSI::DPF::DPFKeyEarlyTerminal_ByArray oa;
    PSI::DPF::DPFKeyEarlyTerminal_ByArray ob;
    PSI::DPF::pcGGM::GenKey(200,1,oa,ob);
    auto ansoa = PSI::DPF::pcGGM::GenTree(oa);
    auto ansob = PSI::DPF::pcGGM::GenTree(ob);

    std::cout << (PSI::bitcheck((uint8_t*)&ansc,2) ^ PSI::bitcheck((uint8_t*)&ansd,2)) << std::endl; 


    
    for(size_t idx = 0; idx < PSI::DPF::DPF_OUTPUT_U64COUNT; idx++){


        // std::cout <<(std::bitset<64>(ansa[idx] ^ansb[idx] ) ==  std::bitset<64>(ansc[idx] ^ansd[idx] ))<< std::endl;
        std::cout <<std::bitset<64>(ansa[idx]) << std::endl;
        std::cout <<std::bitset<64>(ansb[idx]) << std::endl;
        std::cout <<std::bitset<64>(ansb[idx]^ ansa[idx]) << std::endl;


    }
    


    std::cout << "DPF.Genb: "        <<  buildTb.count() << "sec" << std::endl;
    std::cout << "DPF.Evalb: "       << evalTb.count() << "sec" << std::endl;

    std::cout << "DPF.Gen: "        <<  buildT.count() << "sec" << std::endl;
    std::cout << "DPF.Eval: "       << evalT.count() << "sec" << std::endl;
    // std::cout << ansa << std::endl;

    // std::cout << ansb << std::endl;
    // std::cout << (ansa^ansb) << std::endl;
    // // std::cout << ansc << std::endl;
    // // std::cout << ansd << std::endl;
    std::cout << (anse^ansf) << std::endl;
    std::cout << (ansoa^ansob) << std::endl;

   // std::cout <<( (ansa^ansb) == (ansc^ansd) ) << std::endl;
}

void testbitop(){

    __m128i data =  _mm_set_epi64x(0,1);
    uint8_t* datau8 = (uint8_t*)&data;
    if((*datau8) & 1) std::cout << "true " << std::endl;
    __m128i & HCW = data;
    uint8_t temp = HCW[0];

    HCW[0] = 4;
    PSI::util::printchar((uint8_t*)&data,16);

    std::cout <<(temp == 1) << std::endl;
    PSI::util::printchar(&temp,1);
    PSI::Bit_xor1_bytes((uint8_t*)&data,0);
    PSI::util::printchar((uint8_t*)&data,16);

    data ^= PSI::DPF::pcGGM::LSB1Block;
    PSI::util::printchar((uint8_t*)&data,16);
    droidCrypto::block inputcpy = data;
    inputcpy += PSI::DPF::pcGGM::LSB1Block;
    PSI::util::printchar((uint8_t*)&inputcpy,16);
    PSI::util::printchar((uint8_t*)&data,16);

}