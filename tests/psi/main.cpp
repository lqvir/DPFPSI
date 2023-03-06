#include <iostream>
#include "testoprf.hpp"
#include "testdpf.hpp"
#include "test_server.hpp"
int main(){
    PSI::ThreadPoolMgr::SetThreadCount(4);

    test_unbanlanced();

    std::cout<<"hello"<<std::endl;
    return 0;
}