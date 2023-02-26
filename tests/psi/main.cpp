#include <iostream>
#include "testoprf.hpp"
#include "testdpf.hpp"

int main(){
    PSI::ThreadPoolMgr::SetThreadCount(4);

    test_dpf();

    std::cout<<"hello"<<std::endl;
    return 0;
}