#pragma once
#include "psi/common/stopwatch.h"
#include "psi/common/item.h"
#include <ctime>
void test_stopwatch(){
    PSI::StopWatch watch("test");

    watch.setpoint("point1");
    watch.setDurationStart("dur1");

    // usleep(3000);
    watch.setpoint("point2");
    watch.setDurationEnd("dur1");

    watch.printTimePointRecord();
    watch.printDurationRecord();

}
void test_bitvec(){
    PSI::Item t(1,11);
    std::cout<<"bit"<<std::endl;
    for(size_t idx = 0; idx < 127; idx++){
        std::cout<<t.test(idx)<<std::endl;
    }
}