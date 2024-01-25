#pragma once
#include <iostream>
#include <future>

#include "psi/dpf/dpf_server.h"
#include "psi/dpf/dpf_client.h"
#include "psi/dpf/dpf_server.h"
#include "psi/dpf/pcGGM.h"
#include "psi/common/thread_pool_mgr.h"
#include "macoro/wrap.h"
#include <span>
#include "macoro/macros.h"
#include "macoro/thread_pool.h"

#include "coproto/Socket/AsioSocket.h"
using namespace PSI;
using namespace std;

void send_cop(){
    auto chl = coproto::asioConnect("localhost:45000",true);
    PSI::DPF::dpf_server test;
    PSI::DPF::DPFKeyEarlyTerminal_ByArray k0,k1;
    
    MC_BEGIN(macoro::task<>,
        size = size_t{},
        &chl
    );

    test.Gen(23,1,k0,k1);
    chl.send(1);
    chl.send(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&k0),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArray)));
    chl.send(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&k1),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArray)));
    MC_END();

    chl.close();
}
void recv_cop(){
    auto chl = coproto::asioConnect("localhost:45000",false);
    PSI::DPF::DPFKeyEarlyTerminal_ByArray k0,k1;

    MC_BEGIN(
        macoro::task<>,
        x = size_t{},
        &chl
    );
    size_t x;
    MC_AWAIT(chl.recv(x));

    cout << x << endl;
    chl.recv(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&k0),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArray)));
    chl.recv(std::span<uint8_t>(reinterpret_cast<uint8_t*>(&k1),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArray)));
    chl.close();

    auto tree0ee = PSI::DPF::pcGGM::GenTree(k0);
    auto tree1ee = PSI::DPF::pcGGM::GenTree(k1);

    for(size_t idx = 0; idx < PSI::cuckoo::block_size;idx++){
  
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,x1,x2,test.reconstruct(x1,x2));
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0[idx],(uint8_t)tree1[idx],test.reconstruct(tree0[idx],tree1[idx]));
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0e[idx],(uint8_t)tree1e[idx],test.reconstruct(tree0e[idx],tree1e[idx]));
        printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0ee[idx],(uint8_t)tree1ee[idx],tree0ee[idx]^tree1ee[idx]);

    }
    MC_END();
}

void run_test_cop(){
    macoro::sync_wait(macoro::when_all_ready(send_cop,recv_cop));

}