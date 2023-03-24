#pragma once

#include "psi/aid_server.h"
#include "psi/server.h"
#include "psi/client.h"
#include "psi/common/stopwatch.h"

void test_server(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    std::vector<PSI::Label> label;
    PSI::Label test(std::array<uint8_t,PSI::Label_byte_size>{1,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,PSI::Label_byte_size>{1,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,PSI::Label_byte_size>{2,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,PSI::Label_byte_size>{3,2,3,4,5,6,7,8});

    PSI::Server::PSIServer server(1);
    server.init(items,label);

}
void test_cuckoo(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    items.emplace_back(0x125,0x458);
    items.emplace_back(0x128,0x459);

    std::vector<PSI::Label> label(3);
    label[0] = std::string("0012345");
    label[1] = std::string("1012345");
    label[2] = std::string("2012345");

    PSI::Server::PSIServer server(3);
    PSI::Client::PSIClient client(3);

    server.init(items,label);
    auto query = client.OPRFQuery(items);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    client.Cuckoo_All_location(value);

}

void test_DPF(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    items.emplace_back(0x125,0x458);
    items.emplace_back(0x128,0x459);

    std::vector<PSI::Label> label(3);
    label[0] = std::string("0012345");
    label[1] = std::string("1012345");
    label[2] = std::string("2012345");

    PSI::Server::PSIServer server(3);
    PSI::Client::PSIClient client(3);
    PSI::AidServer::AidServer aidserver;

    auto hash_table = server.init(items,label);
    
    auto query = client.OPRFQuery(items);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    client.Cuckoo_All_location(value);
    // PSI::DPF::DPFKeyList ks,ka;
    // client.DPFGen(ks,ka);


    // client.DictGen(response_s,response_a);
    client.InsectionCheck(value,items);
}

void test_unbanlanced(){

    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    for(size_t idx = 101; idx < 101+4 ; idx ++){
        ReceiverSet.emplace_back(0x123+idx,0x456*idx);
        ServerSet.emplace_back(0x123+idx,0x456*idx);   
    }
    
    for(size_t idx = 233; idx < 245 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }

    for(auto x:ServerSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    }
    for(auto x:ReceiverSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    }
    auto sender_size = ServerSet.size();
    auto receiver_size = ReceiverSet.size();
    std::vector<PSI::Label> label(sender_size);
    for(size_t idx = 0 ; idx < sender_size ; idx ++){
        label[idx] = std::string("00000");
    }

    PSI::Server::PSIServer server(sender_size);
    PSI::Client::PSIClient client(receiver_size);
    PSI::AidServer::AidServer aidserver;

    auto hash_table = server.init(ServerSet,label);
    
    auto query = client.OPRFQuery(ReceiverSet);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    client.Cuckoo_All_location(value);
    // aidserver.init(hash_table);
    // PSI::DPF::DPFKeyList ks,ka;
    // client.DPFGen(ks,ka);

    // auto response_s = server.DPFShare(ks);
    // auto response_a = aidserver.DPFShare(ka,hash_table);

  /*  client.DictGen(response_s,response_a);
    client.InsectionCheck(value,ReceiverSet);*/


}
void testXor(){
    PSI::LabelMask a{std::array<uint8_t,PSI::Mask_byte_size>{1,2,3,4,5,6,7,8}};
    PSI::LabelMask b{std::array<uint8_t,PSI::Mask_byte_size>{0}};
    b = PSI::xor_LabelMask(a,b);
    PSI::util::printchar(a.value().data(),16);
}



void test_unbanlancedFullEval(){

    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;

    for(size_t idx = 0; idx < 16 ; idx ++){
        ReceiverSet.emplace_back(0x123+idx,0x456*idx);
        ServerSet.emplace_back(0x123+idx,0x456*idx);   
    }
    
    for(size_t idx = 0; idx < 65520 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }

    for(auto x:ServerSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    }
    for(auto x:ReceiverSet){
        PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    }
    auto sender_size = ServerSet.size();
    auto receiver_size = ReceiverSet.size();
    std::vector<PSI::Label> label(sender_size);
    std::string label_m("00000");
    for(size_t idx = 0 ; idx < sender_size ; idx ++){
        label[idx] = label_m;
        label_m[0] += 1;
        
    }

    PSI::Server::PSIServer server(sender_size);
    PSI::Client::PSIClient client(receiver_size);
    PSI::AidServer::AidServer aidserver;

    auto hash_table = server.init(ServerSet,label);
    
    auto query = client.OPRFQuery(ReceiverSet);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    client.Cuckoo_All_location(value);
    PSI::DPF::DPFKeyList ks,ka;
    client.DPFGen(ks,ka);


    auto response_s = server.DPFShareFullEval(ks);
    auto response_a = aidserver.DPFShareFullEval(ka,hash_table);

    //client.DictGen(response_s,response_a);
    //client.InsectionCheck(value,ReceiverSet);


}

void test_early_terminal(){
    PSI::StopWatch clocks("early");
    std::vector<PSI::Item> ServerSet;
    std::vector<PSI::Item> ReceiverSet;
    clocks.setpoint("start");
    for(size_t idx = 0; idx < 5000 ; idx ++){
        ReceiverSet.emplace_back(0x123+idx,0x456*idx);
        ServerSet.emplace_back(0x123+idx,0x456*idx);   
    }
    
    for(size_t idx = 0; idx < 50000 ; idx ++){
        ServerSet.emplace_back(0x789+idx,0xABC*idx);   
    }

    // for(auto x:ServerSet){
    //     PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    // }
    // for(auto x:ReceiverSet){
    //     PSI::util::printchar(x.get_as<uint8_t>().data(),16);
    // }
    auto sender_size = ServerSet.size();
    auto receiver_size = ReceiverSet.size();
    std::vector<PSI::Label> label(sender_size);
    std::string label_m("00000");
    for(size_t idx = 0 ; idx < sender_size ; idx ++){
        label[idx] = label_m;
        label_m[0] += 1;
        
    }
    clocks.setpoint("prepare data finish");
    std::cout << __LINE__ << std::endl;

    PSI::Server::PSIServer server(sender_size);
    PSI::Client::PSIClient client(receiver_size);
    PSI::AidServer::AidServer aidserver;
    auto hash_table = server.init(ServerSet,label);
    clocks.setpoint("prepare data finish");

    auto query = client.OPRFQuery(ReceiverSet);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    clocks.setpoint("oprf finish ");
    std::cout << __LINE__ << std::endl;

    client.Cuckoo_All_location(value);
    std::cout << __LINE__ << std::endl;

    clocks.setpoint("cuckoo finish ");

    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ks = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
    std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();


    client.DPFGen(ks,ka);
    clocks.setpoint("key generate finish ");
    auto response_s = server.DPFShareFullEval(ks);
    auto response_a = aidserver.DPFShareFullEval(ka, hash_table);

    clocks.setpoint("Full Eval finish ");

    client.DictGen(response_s,response_a);
    client.InsectionCheck(value,ReceiverSet);
    clocks.setpoint("ALL Eval finish ");
    clocks.printTimePointRecord();
}
void Test0(){
    PSI::StopWatch clocks("Test0");

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
    clocks.setpoint("prepare_data_finish");  
    clocks.setDurationStart("offline");
    PSI::Server::PSIServer server(Ssize);
    PSI::AidServer::AidServer aidserver;
    auto hash_table = server.init(ServerSet,label);
    clocks.setDurationEnd("offline");

    for(size_t round_idx = 0; round_idx <= 10; round_idx++){
        std::string round_name = "round"+std::to_string(round_idx);
        clocks.setpoint(round_name+"start");
        for(size_t idx = 0; idx < 32;idx++){
            ReceiverSet[idx*5+round_idx*11] = ServerSet[idx*5+round_idx*11];
        }
        PSI::Client::PSIClient client(Rsize);

        clocks.setDurationStart(round_name);
        clocks.setpoint(round_name+"preprae data finish");
        auto query = client.OPRFQueryThread(ReceiverSet);
        clocks.setpoint(round_name+"oprf finish1");

        auto response = server.process_query_thread(query);
        clocks.setpoint(round_name+"oprf finish2");

        auto value = client.OPRFResponseThread(response);
        clocks.setpoint(round_name+"oprf finish3");
        
        client.Cuckoo_All_location(value);

        clocks.setpoint(round_name+"cuckoo finish ");
        
        std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ks = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
        std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_shared<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
        client.DPFGen(ks,ka);
        clocks.setpoint(round_name+"key generate finish ");

        auto response_s = server.DPFShareFullEval(ks);
        auto response_a = aidserver.DPFShareFullEval(ka, hash_table);

        clocks.setpoint(round_name+"Full Eval finish ");

        client.DictGen(response_s,response_a);
        client.InsectionCheck(value,ReceiverSet);
        clocks.setpoint(round_name+"ALL Eval finish ");

        clocks.setDurationEnd(round_name);

    }

    clocks.printTimePointRecord();

    std::cout<<std::endl;
    std::cout<<std::endl;
    std::cout<<std::endl;

    clocks.printDurationRecord();
}