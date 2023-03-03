#pragma once

#include "psi/server.h"
#include "psi/client.h"
void test_server(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    std::vector<PSI::Label> label;
    label.emplace_back((0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8));
    PSI::Server::PSIServer server(1);
    server.init(items,label);

}
void test_cuckoo(){
    std::vector<PSI::Item> items;
    items.emplace_back(0x123,0x456);
    items.emplace_back(0x125,0x458);
    items.emplace_back(0x128,0x459);

    std::vector<PSI::Label> label;
    label.emplace_back((0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8));
    label.emplace_back((0x2,0x2,0x3,0x4,0x5,0x6,0x7,0x8));
    label.emplace_back((0x3,0x2,0x3,0x4,0x5,0x6,0x7,0x8));
    PSI::Server::PSIServer server(3);
    PSI::Client::PSIClient client(3);

    server.init(items,label);
    auto query = client.OPRFQuery(items);
    auto response = server.process_query(query);
    auto value = client.OPRFResponse(response);
    client.Cuckoo_All_location(value);

}