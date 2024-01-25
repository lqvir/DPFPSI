#include <iomanip>
#include "stopwatch.h"

namespace PSI
{
    using namespace std::chrono;

    StopWatch::StopWatch(const std::string& StopWatchName):Watch_name(StopWatchName){}

    void StopWatch::setpoint(const std::string& message){
        TimeTables.emplace_back(message,steady_clock::now());
    }
    void StopWatch::setDurationStart(const std::string& message){
        TimePoints[message] = steady_clock::now();
    }
    void StopWatch::setDurationEnd(const std::string& message){
        auto search = TimePoints.find(message);
        if(search == TimePoints.end()){
            throw std::runtime_error("Need a Duration Start");
        }
        DuraList.emplace_back(message,steady_clock::now() - search->second);
    }

    void StopWatch::printTimePointRecord(){
        std::cout << Watch_name << "'s Time Point Record" <<std::endl;
        size_t width = 0;
        size_t p = 9;
        for(auto x : TimeTables){
            width = std::max(width,x.first.size());
        }
        width += 3;

        std::cout << std::left << std::setw(width) << "Label  " << "  " << std::setw(p) << "Time (ms)" << "  " << std::setw(p) << "diff (ms)\n__________________________________"  << std::endl;


        for(auto iter = TimeTables.begin()+1,prev = TimeTables.begin();iter != TimeTables.end(); iter++,prev++){
            auto time = duration_cast<std::chrono::microseconds>(iter->second - TimeTables.begin()->second).count() / 1000.0;
            auto diff = duration_cast<std::chrono::microseconds>(iter->second - prev->second).count() / 1000.0;

            std::cout << std::setw(width) << std::left << iter->first
                << "  " << std::right << std::fixed << std::setprecision(1) << std::setw(p) << time
                << "  " << std::right << std::fixed << std::setprecision(3) << std::setw(p) << diff
                << std::endl;;
        }
    }

    void StopWatch::printDurationRecord(){
        std::cout << Watch_name << "'s Duration Record" <<std::endl;
        size_t width = 0;
        size_t p = 9;
        for(auto x : DuraList){
            width = std::max(width,x.first.size());
        }

        std::cout << std::left << std::setw(width) << "Label  " << "  " <<  std::setw(p) << "diff (ms)\n__________________"  << std::endl;
        for(auto x: DuraList){
             std::cout << std::setw(width) << std::left << x.first
                << "  " << std::right << std::fixed << std::setprecision(1) << std::setw(p) << (duration_cast<std::chrono::microseconds>(x.second).count()/1000.0)
                << std::endl;
        }
    }
} // namespace PSI
