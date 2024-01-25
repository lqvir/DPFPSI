#include <iostream>
#include "psi/common/Network/Session.h"
#include "psi/common/Network/IOService.h"
#include "psi/common/Network/Channel.h"
#include <future>
#include "psi/dpf/dpf_client.h"
#include "psi/dpf/dpf_server.h"
#include "psi/dpf/pcGGM.h"

#include "psi/common/thread_pool_mgr.h"
using namespace PSI;
void server_func(){
    IOService ios;
    Session session(ios,"127.0.0.1",SessionMode::Server);
    Channel chl;
    chl = session.addChannel();
    uint64_t temp = 100;
    chl.send(temp);
    std::cout << "ss" << chl.getTotalDataSent() << std::endl;
    std::cout << "sr" << chl.getTotalDataRecv() << std::endl;
    chl.close();
    session.stop();
    ios.stop();
    
}
void client_func(){
    IOService ios;
    Session session(ios,"127.0.0.1",SessionMode::Client);
    Channel chl;
    chl = session.addChannel();
    uint64_t temp;
    chl.recv(temp);
    std::cout <<"receive" <<temp << std::endl;
    std::cout <<"cs" << chl.getTotalDataSent() << std::endl;
    std::cout <<"cr" <<  chl.getTotalDataRecv() << std::endl;

    chl.close();
    session.stop();
    ios.stop();
}

void server_func_dpf(){
    IOService ios;
    Session session(ios,"127.0.0.1:40000",SessionMode::Server);
    Channel* chl = new Channel;
    *chl = session.addChannel();
    PSI::DPF::dpf_server test;
    PSI::DPF::DPFKeyEarlyTerminal_ByArray k0,k1;
    
    test.Gen(23,1,k0,k1);
    auto send_lambda = [&](Channel* chl){
        chl->send(k0.share);
        chl->send(k0.cw);
        chl->send(k0.cw_n1);
        chl->send(k0.cw_n_HCW);
        chl->send(k0.cw_n_last);
    };

    send_lambda(chl);

    chl->send(k1.share);
    chl->send(k1.cw);
    chl->send(k1.cw_n1);
    chl->send(k1.cw_n_HCW);
    chl->send(k1.cw_n_last);

    
    chl->close();
    session.stop();
    ios.stop();
}

void client_func_dpf(){
    IOService ios;
    Session session(ios,"127.0.0.1:40000",SessionMode::Client);
    Channel* chl = new Channel;
    *chl = session.addChannel();
    PSI::DPF::DPFKeyEarlyTerminal_ByArray k0,k1;
    chl->recv(k0.share);
    chl->recv(k0.cw);
    chl->recv(k0.cw_n1);
    chl->recv(k0.cw_n_HCW);
    chl->recv(k0.cw_n_last);

    chl->recv(k1.share);
    chl->recv(k1.cw);
    chl->recv(k1.cw_n1);
    chl->recv(k1.cw_n_HCW);
    chl->recv(k1.cw_n_last);

    auto tree0ee = PSI::DPF::pcGGM::GenTree(k0);
    auto tree1ee = PSI::DPF::pcGGM::GenTree(k1);

    for(size_t idx = 0; idx < PSI::cuckoo::block_size;idx++){
  
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,x1,x2,test.reconstruct(x1,x2));
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0[idx],(uint8_t)tree1[idx],test.reconstruct(tree0[idx],tree1[idx]));
        // printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0e[idx],(uint8_t)tree1e[idx],test.reconstruct(tree0e[idx],tree1e[idx]));
        printf("idx : %d, x1 : %d x2 : %d ans %d; \n",idx,(uint8_t)tree0ee[idx],(uint8_t)tree1ee[idx],tree0ee[idx]^tree1ee[idx]);

    }


    chl->close();
    session.stop();
    ios.stop();
}

void threads_send_func(){
    ThreadPoolMgr tpm;
    std::vector<std::future<void>> futures(ThreadPoolMgr::GetThreadCount());
    IOService ios;
    Session session(ios,"127.0.0.1:40000",SessionMode::Server);
    std::vector<Channel> chls(ThreadPoolMgr::GetThreadCount());
    for(auto &chl : chls){
        chl = session.addChannel();
    }
    int flag[4] = {1,2,3,4};
    auto send_lambda = [&](size_t idx){
        chls[idx].send(flag[idx]);
    };
    for(size_t idx = 0; idx < ThreadPoolMgr::GetThreadCount();idx++){
        futures[idx] = tpm.thread_pool().enqueue(send_lambda,idx);
    }
    for(auto &x : futures){
        x.get();
    }

    for(auto &x :chls){
        x.close();
    }
    session.stop();
    ios.stop();
}
void threads_recv_func(){
    ThreadPoolMgr tpm;
    std::vector<std::future<void>> futures(ThreadPoolMgr::GetThreadCount());
    IOService ios;
    Session session(ios,"127.0.0.1:40000",SessionMode::Client);
    std::vector<Channel> chls(ThreadPoolMgr::GetThreadCount());
    for(auto &chl : chls){
        chl = session.addChannel();
    }
    int flag[4];
    auto send_lambda = [&](size_t idx){
        chls[idx].recv(flag[idx]);
    };
    for(size_t idx = 0; idx < ThreadPoolMgr::GetThreadCount();idx++){
        futures[idx] = tpm.thread_pool().enqueue(send_lambda,idx);
    }
    for(auto &x : futures){
        x.get();
    }
    for(auto x:flag){
        std::cout << x << std::endl;
    }
    for(auto &x :chls){
        x.close();
    }
    session.stop();
    ios.stop();
}
void run_test(){
    std::future<void> threads_server = std::async(threads_send_func);
    threads_recv_func();
    threads_server.get();

}