#include <iostream>
#include <string>
#include <cstdlib>
#include "Insection.h"
#include "psi/aid_server.h"
#include "psi/common/stopwatch.h"
#include "psi/common/thread_pool_mgr.h"

void printHelp() {
    std::cout << "Usage: Client_cli " << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help     Display this help message" << std::endl;
}
struct clientparams{
    size_t threads;
    size_t setsize;
    size_t logsetsize;
    size_t inssize=1;
}cmdparams;

int PerfromCMD(int argc, char* argv[]){
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-t" || arg == "--threads") {
            if (i + 1 < argc) {
                cmdparams.threads = std::atoi(argv[i + 1]);
                ++i;
            } else {
                std::cerr << "Error: No number provided after " << arg << std::endl;
                return 1;
            }
        } else if (arg == "-l" || arg == "--log"){
            if (i + 1 < argc) {
                cmdparams.logsetsize =  std::atoi(argv[i + 1]);
                cmdparams.setsize = 1 << cmdparams.logsetsize;
                ++i;
            } else {
                std::cerr << "Error: No number provided after " << arg << std::endl;
                return 1;
            }
        } else if (arg == "-i" || arg == "--ins"){
            if (i + 1 < argc) {
                cmdparams.inssize = std::atoi(argv[i + 1]);
                ++i;
            } else {
                std::cerr << "Error: No number provided after " << arg << std::endl;
                return 1;
            }
        }
        else {
            printHelp();
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return 1;
        }
    }

    std::cout << "threads: " << cmdparams.threads << std::endl;
    std::cout << "Client Set size: " << cmdparams.setsize << "(2^"<< cmdparams.logsetsize << ")" << std::endl;
    std::cout << "Insection Size: " << cmdparams.inssize << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {

    if(PerfromCMD(argc,argv))
        return 1;
   

    std::cout << "Prepare TestData Finish" << std::endl;
    PSI::StopWatch aidserverclocks("aidserver");
    
    PSI::ThreadPoolMgr::SetThreadCount(cmdparams.threads);

    PSI::AidServer::AidServer aidserver;
    aidserverclocks.setpoint("start");
    aidserver.DHBasedPSI_start("127.0.0.1:50002","127.0.0.1:50001");

    aidserverclocks.setpoint("ALL Finish");

    std::cout << "------------------- ALL Server Finish ---------------------" << std::endl;

    aidserverclocks.printTimePointRecord();
    return 0;
}
