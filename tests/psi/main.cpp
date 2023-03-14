#include <iostream>
#include "testoprf.hpp"
#include "testdpf.hpp"
#include "test_server.hpp"
#include "test_stopwatch.hpp"
int main(){
    PSI::ThreadPoolMgr::SetThreadCount(1);
    PSI::StopWatch clock1;
    clock1.setpoint("start");
    test_early_terminal();
    clock1.setpoint("finish early terminal");
    // test_unbanlancedFullEval();
    // clock1.setpoint("finish without early terminal");

    std::cout<<"hello"<<std::endl;
    // clock1.printTimePointRecord();
    return 0;
}