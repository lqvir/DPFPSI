#pragma once

#include "psi/aid_server.h"
#include "psi/server.h"
#include "psi/client.h"
#include "psi/common/stopwatch.h"
#include <algorithm>



void testXor(){
    PSI::LabelMask a{std::array<uint8_t,PSI::Mask_byte_size>{1,2,3,4,5,6,7,8}};
    PSI::LabelMask b{std::array<uint8_t,PSI::Mask_byte_size>{0}};
    b = PSI::xor_LabelMask(a,b);
    PSI::util::printchar(a.value().data(),16);
}





#include "psi/common/Network/Session.h"
#include "psi/common/Network/IOService.h"
#include "psi/common/Network/Channel.h"
#include "psi/common/thread_pool_mgr.h"
void Test2(){
    PSI::StopWatch clocks("Test1");

    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    size_t Rsize = 5535;
    size_t Ssize = 65536;
    std::vector<PSI::Label> label(Ssize);
    clocks.setpoint("start");  
    for(size_t idx = 0;idx < Ssize; idx++){
        uint64_t temp[2];
        RAND_bytes((uint8_t*)temp,16);
        ServerSet.emplace_back(temp[0],temp[1]);
        RAND_bytes((uint8_t*)temp,16);
        label.emplace_back(temp[0],temp[1]);
    }
    for(size_t idx = 0; idx < Rsize; idx++){
        uint64_t temp[2];
        RAND_bytes((uint8_t*)temp,16);
        ReceiverSet.emplace_back(temp[0],temp[1]);
    }

    PSI::IOService iosS;
    PSI::IOService iosA;
    PSI::Session sessionS(iosS,"127.0.0.1:50000",PSI::SessionMode::Server);
    std::vector<PSI::Channel> chlsS(PSI::ThreadPoolMgr::GetThreadCount());
    for(auto &chl : chlsS){
        chl = sessionS.addChannel();
    }
    PSI::Session sessionA(iosA,"127.0.0.1:50001",PSI::SessionMode::Server);
    std::vector<PSI::Channel> chlsA(PSI::ThreadPoolMgr::GetThreadCount());
    for(auto &chl : chlsA){
        chl = sessionA.addChannel();
    }
    for(size_t idx = 0; idx < 32;idx++){
        ReceiverSet[idx*5+7*11] = ServerSet[idx*5+7*11];
    }
    auto lambdaClient = [&](){
        droidCrypto::CSocketChannel chanc("127.0.0.1", 8000, false);

        PSI::Client::PSIClient client(Rsize,chanc);
        client.DHBasedPSI_start("127.0.0.1:50000","127.0.0.1:50001",ReceiverSet);

    };

    droidCrypto::CSocketChannel chans("127.0.0.1", 8000, true);

    PSI::Server::PSIServer server(Ssize,chans);
    auto hash_table = server.init_FourQ(ServerSet,label);
    auto lambdaAidServer = [&](){
        PSI::AidServer::AidServer aidserver;
        aidserver.run(hash_table,chlsA);
    };
    auto threads = std::async(lambdaAidServer);

    auto threadc = std::async(lambdaClient);
    server.runDH(chlsS);


    threadc.get();
    threads.get();
    size_t comm_S_send=0;
    size_t comm_S_recv=0;
    size_t comm_A_send=0;
    size_t comm_A_recv=0;

    for(auto x : chlsS){
        comm_S_send+= x.getTotalDataSent();
        comm_S_recv+= x.getTotalDataRecv();
    }
    for(auto x : chlsA){
        comm_A_send+= x.getTotalDataSent();
        comm_A_recv+= x.getTotalDataRecv();
    }
    std::cout << "S send" <<1.0*comm_S_send/1024/1024 << "S recv" << 1.0*comm_S_recv/1024/1024 << std::endl;
    std::cout << "A send" <<1.0*comm_A_send/1024/1024 << "A recv" << 1.0*comm_A_send/1024/1024 << std::endl;
    for(auto &chl : chlsA){
        chl.close();
    }        
    for(auto &chl : chlsS){
        chl.close();
    }            
    sessionA.stop();

    sessionS.stop();
    iosS.stop();
    iosA.stop();

}

void Test3(){
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    size_t Rsize = 1024;
    size_t Ssize = 1048576;
    std::vector<PSI::Label> label(Ssize);
        for(size_t idx = 0;idx < Ssize; idx++){
            uint64_t temp[2];
            RAND_bytes((uint8_t*)temp,16);
            ServerSet.emplace_back(temp[0],temp[1]);
            if(PSI::Label_byte_size != 0){
                RAND_bytes((uint8_t*)temp,16);
                label.emplace_back(temp[0],temp[1]);
            }   
        }
    for(size_t idx = 0; idx < Rsize; idx++){
        uint64_t temp[2];
        RAND_bytes((uint8_t*)temp,16);
        ReceiverSet.emplace_back(temp[0],temp[1]);
    }
    // ReceiverSet[0] = ServerSet[0];
    for(size_t idx = 0; idx < 32;idx++){
        ReceiverSet[idx*5+7*11] = ServerSet[idx*5+7*11];
    }
    
    auto lambdaClient = [&](){
        droidCrypto::CSocketChannel chanc("127.0.0.1", 8000, false);

        PSI::Client::PSIClient client(Rsize,chanc);

        client.DHBasedPSI_start("127.0.0.1:50000","127.0.0.1:50001",ReceiverSet);

    };
    auto lambdaAidServer = [&](){
        PSI::AidServer::AidServer aidserver;
        aidserver.DHBasedPSI_start("127.0.0.1:50002","127.0.0.1:50001");
    };
    auto threads = std::async(lambdaAidServer);

    auto threadc = std::async(lambdaClient);
    droidCrypto::CSocketChannel chans("127.0.0.1", 8000, true);
    
    PSI::Server::PSIServer server(Ssize,chans);

    server.DHBasedPSI_start("127.0.0.1:50000","127.0.0.1:50002",ServerSet,label);
    threadc.get();
    threads.get();

}

void TestGC(){
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    size_t Rsize = 1024;
    size_t Ssize = 1048576;
    std::vector<PSI::Label> label(Ssize);
        for(size_t idx = 0;idx < Ssize; idx++){
            uint64_t temp[2];
            RAND_bytes((uint8_t*)temp,16);
            ServerSet.emplace_back(temp[0],temp[1]);
            if(PSI::Label_byte_size != 0){
                RAND_bytes((uint8_t*)temp,16);
                label.emplace_back(temp[0],temp[1]);
            }   
        }
    for(size_t idx = 0; idx < Rsize; idx++){
        uint64_t temp[2];
        RAND_bytes((uint8_t*)temp,16);
        ReceiverSet.emplace_back(temp[0],temp[1]);
    }
    // ReceiverSet[0] = ServerSet[0];
    for(size_t idx = 0; idx < 32;idx++){
        ReceiverSet[idx*5+7*11] = ServerSet[idx*5+7*11];
    }
    
    auto lambdaClient = [&](){
        droidCrypto::CSocketChannel chanc("127.0.0.1", 8000, false);

        PSI::Client::PSIClient client(Rsize,chanc);

        client.GCBasedPSI_start("127.0.0.1:50000","127.0.0.1:50001",ReceiverSet);

    };
    auto lambdaAidServer = [&](){
        PSI::AidServer::AidServer aidserver;
        aidserver.GCBasedPSI_start("127.0.0.1:50002","127.0.0.1:50001");
    };
    auto threads = std::async(lambdaAidServer);

    auto threadc = std::async(lambdaClient);
    droidCrypto::CSocketChannel chans("127.0.0.1", 8000, true);
    
    PSI::Server::PSIServer server(Ssize,chans);

    server.GCBasedPSI_start("127.0.0.1:50000","127.0.0.1:50002",ServerSet,label);
    threadc.get();
    threads.get();

}