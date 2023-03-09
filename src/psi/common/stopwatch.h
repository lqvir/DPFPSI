#pragma once
#include <chrono>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
namespace PSI{
    class StopWatch{
    public:
        StopWatch() = default;
        StopWatch(const std::string& Watch_name);
        void setpoint(const std::string& message);
        void setDurationStart(const std::string& message);
        void setDurationEnd(const std::string& message);
        void printTimePointRecord();
        void printDurationRecord();

    private:
            std::string Watch_name;
            std::vector<std::pair<std::string,std::chrono::steady_clock::time_point> > TimeTables;
            std::vector<std::pair<std::string,std::chrono::steady_clock::duration> > DuraList;

            std::unordered_map<std::string,std::chrono::steady_clock::time_point> TimePoints;
            
    };



} // namespace PSI