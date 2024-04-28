#include <iostream>
#include "testoprf.hpp"
#include "testdpf.hpp"
#include "test_server.hpp"
#include "test_stopwatch.hpp"
#include "test_network.hpp"
#include "test_cuckoohash.hpp"
// #include "catch2/catch_test_macros.hpp"
// static int Factorial( int number ) {
//    return number <= 1 ? number : Factorial( number - 1 ) * number;  // fail
// // return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
// }

// TEST_CASE( "Factorial of 0 is 1 (fail)", "[single-file]" ) {
//     REQUIRE( Factorial(0) == 1 );
// }

// TEST_CASE( "Factorials of 1 and higher are computed (pass)", "[single-file]" ) {
//     REQUIRE( Factorial(1) == 1 );
//     REQUIRE( Factorial(2) == 2 );
//     REQUIRE( Factorial(3) == 6 );
//     REQUIRE( Factorial(10) == 3628800 );
// }

int main(int argc,char* argv[]){
    auto threadnum = std::atoi(argv[1]);
    std::cout << threadnum << std::endl;
    PSI::ThreadPoolMgr::SetThreadCount(threadnum);
    PSI::StopWatch clock1("All time ");
    clock1.setpoint("start");
    // Test0();

    TestDHSIMD();
    clock1.setpoint("finish early terminal");
    // test_unbanlancedFullEval();
    // clock1.setpoint("finish without early terminal");
    std::cout<<sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList) << std::endl;
    std::cout<<"hello"<<std::endl;
    clock1.printTimePointRecord();
    return 0;
}