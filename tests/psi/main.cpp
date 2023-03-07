#include <iostream>
#include "testoprf.hpp"
#include "testdpf.hpp"
#include "test_server.hpp"
#include "test_stopwatch.hpp"
int main(){
    PSI::ThreadPoolMgr::SetThreadCount(4);

    test_stopwatch();

    std::cout<<"hello"<<std::endl;
    return 0;
}