#pragma once

#include "psi/aid_server.h"
#include "psi/server.h"
#include "psi/client.h"
void test_server(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    std::vector<PSI::Label> label;
    PSI::Label test(std::array<uint8_t,8>{1,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,8>{1,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,8>{2,2,3,4,5,6,7,8});
    label.emplace_back(std::array<uint8_t,8>{3,2,3,4,5,6,7,8});

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
    aidserver.init(hash_table);
    client.DPFGen();

    auto ks = client.getKs();
    auto ka = client.getKa();


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
    aidserver.init(hash_table);
    client.DPFGen();

    auto ks = client.getKs();
    auto ka = client.getKa();

    auto response_s = server.DPFShare(ks);
    auto response_a = aidserver.DPFShare(ka,hash_table);

    client.DictGen(response_s,response_a);
    client.InsectionCheck(value,ReceiverSet);


}
void testXor(){
    PSI::LabelMask a{std::array<uint8_t,16>{1,2,3,4,5,6,7,8}};
    PSI::LabelMask b{std::array<uint8_t,16>{0}};
    b = PSI::xor_LabelMask(a,b);
    PSI::util::printchar(a.value().data(),16);
}

void test_FUllEval(){
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
    aidserver.init(hash_table);
    client.DPFGen();

    auto ks = client.getKs();
    auto ka = client.getKa();

    auto response_s = server.DPFShare(ks);
    auto response_a = aidserver.DPFShare(ka,hash_table);
    
    auto check_s = server.DPFShareFullEval(ks);
    auto check_a = aidserver.DPFShareFullEval(ka,hash_table);

    for(size_t idx = 0; idx < PSI::cuckoo::block_num; idx ++){
        for(size_t mask_idx = 0; mask_idx < PSI::cuckoo::max_set_size; mask_idx++){
            std::cout << idx << ' ' << mask_idx << std::endl;
            std::cout << (response_s.at(idx).at(mask_idx) == check_s.at(idx).at(mask_idx)) << ' ';
            std::cout << (response_a.at(idx).at(mask_idx) == check_a.at(idx).at(mask_idx)) << std::endl;

        }
    }
}

void test_unbanlancedFullEval(){

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
    aidserver.init(hash_table);
    client.DPFGen();

    auto ks = client.getKs();
    auto ka = client.getKa();

    auto response_s = server.DPFShareFullEval(ks);
    auto response_a = aidserver.DPFShareFullEval(ka,hash_table);

    client.DictGen(response_s,response_a);
    client.InsectionCheck(value,ReceiverSet);


}