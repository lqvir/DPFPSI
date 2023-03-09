#pragma once
#include "psi/common/stopwatch.h"
#include <ctime>
void test_stopwatch(){
    PSI::StopWatch watch("test");

    watch.setpoint("point1");
    watch.setDurationStart("dur1");

    _sleep(3000);
    watch.setpoint("point2");
    watch.setDurationEnd("dur1");

    watch.printTimePointRecord();
    watch.printDurationRecord();

}
