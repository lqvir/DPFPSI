#include "client.h"
#include "cuckoo_hash/cuckoo.h"
#include <unordered_set>
#include "common/thread_pool_mgr.h"
#include "common/stopwatch.h"
#include "gsl/span"
namespace PSI
{
    namespace Client
    {
        std::vector<DHOPRF::OPRFPoint> PSIClient::OPRFQuery(const std::vector<Item>& input){
            DHOPRFReceiver.init();
            auto OPRFQuery = DHOPRFReceiver.process_items(input);
            return OPRFQuery;
        }
        std::vector<DHOPRF::OPRFPoint> PSIClient::OPRFQueryThread(const std::vector<Item>& input){
            DHOPRFReceiver.init();
            auto OPRFQuery = DHOPRFReceiver.process_items_threads(input);
            return OPRFQuery;
        }
        std::vector<DHOPRF::OPRFValueOpenssL> PSIClient::OPRFResponse(const std::vector<DHOPRF::OPRFPoint>& response){
            auto output = DHOPRFReceiver.process_response(response);
            return output;
        }
        std::vector<DHOPRF::OPRFValueOpenssL> PSIClient::OPRFResponseThread(const std::vector<DHOPRF::OPRFPoint>& response){
            auto output = DHOPRFReceiver.process_response_threads(response);
            return output;
        }

        std::vector<DHOPRF::OPRFPointFourQ> PSIClient::OPRFQueryFourQ(const std::vector<Item>& input){
            DHOPRFReceiver.init();
            auto OPRFQuery = DHOPRFReceiver.process_itemsFourQ(input);
            return OPRFQuery;
        }
        std::vector<DHOPRF::OPRFPointFourQ> PSIClient::OPRFQueryThreadFourQ(const std::vector<Item>& input){
            DHOPRFReceiver.init();
            auto OPRFQuery = DHOPRFReceiver.process_items_threadsFourQ(input);
            return OPRFQuery;
        }
        std::vector<DHOPRF::OPRFValueOpenssL> PSIClient::OPRFResponseFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response){
            auto output = DHOPRFReceiver.process_responseFourQ(response);
            return output;
        }
        std::vector<DHOPRF::OPRFValueOpenssL> PSIClient::OPRFResponseThreadFourQ(const std::vector<DHOPRF::OPRFPointFourQ>& response){
            auto output = DHOPRFReceiver.process_response_threadsFourQ(response);
            return output;
        }





        void PSIClient::Cuckoo_All_location(const std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input){
            std::vector<kuku::LocFunc> hash_funcs;
            for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
                hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
            }
            std::unordered_set<kuku::location_type> IndexSets;
                // std::cout << "oprf_value" <<std::endl;;
            
            for(auto x : oprf_input){
                // util::printchar((uint8_t*)x.data(),x.size());
                for (auto &hf : hash_funcs) {
                    IndexSets.emplace(hf(kuku::make_item((uint8_t*)x.data())));
                }
            }
            // std::cout << __LINE__ << std::endl;
                // for(auto it = IndexSets.begin(); it != IndexSets.end(); it++){
                //     std::cout << *it << std::endl;
                // }
            IndexSets_by_block.resize(cuckoo::block_num);
            for(auto it = IndexSets.begin();it!=IndexSets.end();it++){
                IndexSets_by_block.at(cuckoo::get_block_id(*it)).emplace_back(cuckoo::get_pos(*it));
            }
            for(auto &Indexset: IndexSets_by_block){
                if(Indexset.size()>= cuckoo::max_set_size){
                    std::cout << " The number of elements in the set exceeds the maximum value." << std::endl;
                    for(auto x : IndexSets_by_block){
                        std::cout << x.size() << std::endl;
                    }
                    throw std::runtime_error("Overload");
                }
                // std::cout << std::endl;


                // std::cout << std::endl;
            }


        }
        void PSIClient::Cuckoo_All_location(const std::unique_ptr<std::vector<GCOPRF::GCOPRFValue>>& oprf_input){
            std::vector<kuku::LocFunc> hash_funcs;
            for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
                hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
            }
            std::unordered_set<kuku::location_type> IndexSets;
                // std::cout << "oprf_value" <<std::endl;;
            
            for(auto x : (*oprf_input)){
                // util::printchar((uint8_t*)x.data(),x.size());
                for (auto &hf : hash_funcs) {
                    IndexSets.emplace(hf(kuku::make_item((uint8_t*)x.data())));
                }
            }
            // std::cout << __LINE__ << std::endl;
                // for(auto it = IndexSets.begin(); it != IndexSets.end(); it++){
                //     std::cout << *it << std::endl;
                // }
            IndexSets_by_block.resize(cuckoo::block_num);
            for(auto it = IndexSets.begin();it!=IndexSets.end();it++){
                IndexSets_by_block.at(cuckoo::get_block_id(*it)).emplace_back(cuckoo::get_pos(*it));
            }
            for(auto &Indexset: IndexSets_by_block){
                if(Indexset.size()>= cuckoo::max_set_size){
                    std::cout << " The number of elements in the set exceeds the maximum value." << std::endl;
                    for(auto x : IndexSets_by_block){
                        std::cout << x.size() << std::endl;
                    }
                    throw std::runtime_error("Overload");
                }
                // std::cout << std::endl;


                // std::cout << std::endl;
            }


        }
    // void PSIClient::DPFGen(DPF::DPFKeyList& Ks,DPF::DPFKeyList& Ka){

    //     for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
    //         auto block_size = IndexSets_by_block.at(block_id).size();
    //         for(size_t pos_id = 0; pos_id < block_size; pos_id++){
           
    //             DPFServer.Gen(IndexSets_by_block.at(block_id).at(pos_id),1,Ks.at(block_id).at(pos_id),Ka.at(block_id).at(pos_id));
    //         }
    //         for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
    //             Ka.at(block_id).at(pad_idx).RandomKey();
    //             Ks.at(block_id).at(pad_idx).RandomKey();
    //         }
    //     }
    // }
    // void PSIClient::DPFGen(
    //     std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ks,
    //     std::shared_ptr<PSI::DPF::DPFKeyEarlyTerminalList> Ka
    //               )
    // {

    //     for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
    //         auto block_size = IndexSets_by_block.at(block_id).size();
    //         for(size_t pos_id = 0; pos_id < block_size; pos_id++){
           
    //             DPFServer.Gen(IndexSets_by_block.at(block_id).at(pos_id),1,Ks->at(block_id).at(pos_id),Ka->at(block_id).at(pos_id));
    //         }
    //         for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
    //             Ka->at(block_id).at(pad_idx).RandomKey();
    //             Ks->at(block_id).at(pad_idx).RandomKey();
    //         }
    //     }
    
    // }
    void PSIClient::DPFGen(
        const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> &Ks,
        const std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> &Ka
                  )
    {
        // Ks->resize(cuckoo::block_num);
        // Ka->resize(cuckoo::block_num);
        for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            auto block_size = IndexSets_by_block.at(block_id).size();

            // Ks->at(block_id).resize(block_size);
            // Ka->at(block_id).resize(block_size);

            for(size_t pos_id = 0; pos_id < block_size; pos_id++){
           
                DPF::DPFServer::Gen(IndexSets_by_block.at(block_id).at(pos_id),1,Ks->at(block_id).at(pos_id),Ka->at(block_id).at(pos_id));
            }
            // for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
            //     Ka->at(block_id).at(pad_idx).RandomKey();
            //     Ks->at(block_id).at(pad_idx).RandomKey();
            // }
        }
    
    }

    void PSIClient::DPFGen(
        const std::unique_ptr<PSI::DPF::pcGGM::DPFKeyList> &Ks,
        const std::unique_ptr<PSI::DPF::pcGGM::DPFKeyList> &Ka
                  )
    {
        // Ks->resize(cuckoo::block_num);
        // Ka->resize(cuckoo::block_num);
        for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            auto block_size = IndexSets_by_block.at(block_id).size();

            // Ks->at(block_id).resize(block_size);
            // Ka->at(block_id).resize(block_size);

            for(size_t pos_id = 0; pos_id < block_size; pos_id++){
                DPF::pcGGM::GenKey(IndexSets_by_block.at(block_id).at(pos_id),1,Ks->at(block_id).at(pos_id),Ka->at(block_id).at(pos_id));
            }
            // for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
            //     Ka->at(block_id).at(pad_idx).RandomKey();
            //     Ks->at(block_id).at(pad_idx).RandomKey();
            // }
        }
    
    }

    void PSIClient::DictGen(const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromS,
        const std::unique_ptr<DPF::DPFResponseList>& ResponseListFromA){
        
        
        for(size_t block_id =0 ; block_id < cuckoo::block_num ; block_id ++ ){
            auto& DPFResponseS = ResponseListFromS->at(block_id);
            auto& DPFResponseA = ResponseListFromA->at(block_id);
            auto& Indexset = IndexSets_by_block[block_id];
            size_t block_size_before_padding = IndexSets_by_block.at(block_id).size();
            for(size_t index_id = 0; index_id < block_size_before_padding ; index_id++){
                size_t index = Indexset[index_id];
                auto& LabelMaskS =  DPFResponseS[index_id];
                auto& LabelMaskA =  DPFResponseA[index_id];
                // if((DPFValue&1)){
                //     Dict.insert(std::make_pair(cuckoo::get_BinID(index,block_id),DPFResponseS.cuckoo_table.at(index)));

                //     printf("block_id %d KeyID %d ValueID %d : %02x \n",block_id,index_id,index,DPFValue);

                // }  
                Dict[cuckoo::get_BinID(index,block_id)] = 
                            xor_LabelMask(LabelMaskS,LabelMaskA);

            }
        }

    }
    void PSIClient::InsectionCheck(std::vector<DHOPRF::OPRFValueOpenssL>& oprf_input,const std::vector<Item>& input){
        std::vector<kuku::LocFunc> hash_funcs;
        std::string zero_mask;
        zero_mask.assign(Leading_zero_length,'\0');
        for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
            hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
        }
        std::vector<std::pair<Item,PSI::Label>> Ins;
        for(size_t idx = 0; idx < oprf_input.size(); idx++){
            gsl::span<uint8_t,OPRFValueBytes> x(oprf_input[idx]);
            auto random_mask = x.subspan(16,Mask_byte_size);
            auto oprf_value = x.subspan(0,OPRFKeyBytesNumber);
            for (auto &hf : hash_funcs) {
                auto binID = hf(kuku::make_item((uint8_t*)oprf_value.data()));
                auto search = Dict.find(binID);
                if(search != Dict.end()){
                    auto AnsMask = search->second;
#if LogLevel == 0
                    // std::cout << (ans == search->second) << std::endl;
                    // util::printchar((uint8_t*)ans.data(),ans.size());
                    // util::printchar((uint8_t*)search->second.data(),search->second.size());
                    // util::printchar((uint8_t*)random_mask.data(),random_mask.size());
#endif
                 
                    auto XorLabelMask = xor_LabelMask(AnsMask,random_mask);
#if LogLevel == 0
                    util::printchar((uint8_t*)XorLabelMask.value().data(),XorLabelMask.value().size());
#endif
                    
                    if(0 == memcmp(XorLabelMask.value().data(),ZeroMask,Leading_zero_length)){
                        Ins.emplace_back(input.at(idx),toLabel(XorLabelMask));// 暂时
                        break;
                    } 
                }
            }
        }
        // for(auto x : Ins){
        //     printf("item:\n");
        //     util::printchar(x.first.get_as<uint8_t>().data(),x.first.get_as<uint8_t>().size());
        //     printf("label:\n");
        //     util::printchar(x.second.get_as<uint8_t>().data(),x.second.get_as<uint8_t>().size());
        // }
        std::cout << "Insection Size" << Ins.size() << std::endl;

    }

    void PSIClient::InsectionCheck(const std::unique_ptr<std::vector<PSI::GCOPRF::GCOPRFValue>>& oprf_input,const std::vector<Item>& input){
            std::vector<kuku::LocFunc> hash_funcs;
            std::string zero_mask;
            zero_mask.assign(Leading_zero_length,'\0');
            for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
                hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
            }
            std::vector<std::pair<Item,PSI::Label>> Ins;
            for(size_t idx = 0; idx < oprf_input->size(); idx++){
                auto x = gsl::make_span((*oprf_input)[idx].data(),OPRFValueBytes);
                auto random_mask = x.subspan(OPRFKeyBytesNumber,Mask_byte_size);
                auto oprf_value = x.subspan(0,OPRFKeyBytesNumber);
                for (auto &hf : hash_funcs) {
                    auto binID = hf(kuku::make_item((uint8_t*)oprf_value.data()));
                    auto search = Dict.find(binID);
                    if(search != Dict.end()){
                        auto AnsMask = search->second;
    #if LogLevel == 0
                        // std::cout << (ans == search->second) << std::endl;
                        // util::printchar((uint8_t*)ans.data(),ans.size());
                        // util::printchar((uint8_t*)search->second.data(),search->second.size());
                        // util::printchar((uint8_t*)random_mask.data(),random_mask.size());
    #endif
                    
                        auto XorLabelMask = xor_LabelMask(AnsMask,random_mask);
    #if LogLevel == 0
                        util::printchar((uint8_t*)XorLabelMask.value().data(),XorLabelMask.value().size());
    #endif
                        
                        if(0 == memcmp(XorLabelMask.value().data(),ZeroMask,Leading_zero_length)){
                            Ins.emplace_back(input.at(idx),toLabel(XorLabelMask));// 暂时
                            break;
                        } 
                    }
                }
            }
            // for(auto x : Ins){
            //     printf("item:\n");
            //     util::printchar(x.first.get_as<uint8_t>().data(),x.first.get_as<uint8_t>().size());
            //     printf("label:\n");
            //     util::printchar(x.second.get_as<uint8_t>().data(),x.second.get_as<uint8_t>().size());
            // }
            std::cout << "Insection Size" << Ins.size() << std::endl;

        }

    void PSIClient::DHBasedPSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input){
        StopWatch clocks("PSIClient");
        clocks.setpoint("start");
        IOService iosS;
        IOService iosA;
        Session sessionS(iosS,ServerAddress,SessionMode::Client);
        Session sessionA(iosA,AidServerAddress,SessionMode::Client);
        std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
        std::vector<Channel> chlsA(ThreadPoolMgr::GetThreadCount());

        for(auto &chl : chlsS){
            chl = sessionS.addChannel();
        }
        for(auto &chl : chlsA){
            chl = sessionA.addChannel();
        }

         std::string OK;
        chlsS[0].recv(OK);
        clocks.setpoint("OPRFStart");

        auto query = OPRFQueryThreadFourQ(input);
        chlsS[0].send(query);
        std::vector<DHOPRF::OPRFPointFourQ> response(client_set_size_);

        chlsS[0].recv(response);

        auto oprf_value = OPRFResponseThreadFourQ(response);

        clocks.setpoint("OPRFFinish");

        Cuckoo_All_location(oprf_value);
        // std::cout <<__FILE__<<":" << __LINE__ << std::endl;

        std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ks = std::make_unique<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
        std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_unique<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();

        DPFGen(ks,ka);
        clocks.setpoint("DPFFinish");
        
        chlsA[0].send(reinterpret_cast<uint8_t*>(ka.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));
        chlsS[0].send(reinterpret_cast<uint8_t*>(ks.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));

        std::unique_ptr<PSI::DPF::DPFResponseList> ResponseS = std::make_unique<PSI::DPF::DPFResponseList>();
        std::unique_ptr<PSI::DPF::DPFResponseList> ResponseA = std::make_unique<PSI::DPF::DPFResponseList>();
        chlsS[0].recv(reinterpret_cast<uint8_t*>(ResponseS.get()),sizeof(PSI::DPF::DPFResponseList));
        chlsA[0].recv(reinterpret_cast<uint8_t*>(ResponseA.get()),sizeof(PSI::DPF::DPFResponseList));
        clocks.setpoint("Dict Start");

        DictGen(std::move(ResponseS),std::move(ResponseA));
        clocks.setpoint("Dict Finish");

        InsectionCheck(oprf_value,input);
        clocks.setpoint("Finish");

        clocks.printTimePointRecord();
        size_t cnt = 0 ;
        for(auto chl : chlsS){
            cnt += chl.getTotalDataSent() + chl.getTotalDataRecv();
            chl.close();
        }

        for(auto chl : chlsA){
            cnt += chl.getTotalDataSent() + chl.getTotalDataRecv();
            chl.close();
        }
        std::cout << "online com" << cnt / 1024.0/1024.0 <<std::endl;
        sessionS.stop();
        sessionA.stop();
        iosS.stop();
        iosA.stop();
    }

    void PSIClient::GCBasedPSI_start(std::string ServerAddress,std::string AidServerAddress,const std::vector<Item>& input){
        StopWatch clocks("PSIClient");
        clocks.setpoint("start");
        IOService iosS;
        IOService iosA;
        Session sessionS(iosS,ServerAddress,SessionMode::Client);
        Session sessionA(iosA,AidServerAddress,SessionMode::Client);
        std::vector<Channel> chlsS(ThreadPoolMgr::GetThreadCount());
        std::vector<Channel> chlsA(ThreadPoolMgr::GetThreadCount());

        for(auto &chl : chlsS){
            chl = sessionS.addChannel();
        }
        for(auto &chl : chlsA){
            chl = sessionA.addChannel();
        }
        std::string OK;
        chlsS[0].recv(OK);
        clocks.setpoint("OPRFStart");

        auto inputptr = std::make_unique<std::vector<droidCrypto::block>>();
        inputptr->reserve(input.size());
        for(auto input_item : input){
            inputptr->emplace_back(droidCrypto::toBlock(input_item.get_as<uint8_t>().data()));
        }
        clocks.setDurationStart("online");
        
        GCOPRFReceiver.base(input.size());

        clocks.setpoint("OPRFbase");

        auto oprf_value = GCOPRFReceiver.Online(std::move(inputptr));



        size_t GCOPRFCom = GCOPRFReceiver.getComCost();



        clocks.setpoint("OPRFFinish");

        Cuckoo_All_location(oprf_value);
        // std::cout <<__FILE__<<":" << __LINE__ << std::endl;

        std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ks = std::make_unique<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
        std::unique_ptr<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList> ka = std::make_unique<PSI::DPF::DPFKeyEarlyTerminal_ByArrayList>();
        clocks.setpoint("DPFStart");

        DPFGen(ks,ka);
        clocks.setpoint("DPFFinish");
        
        chlsA[0].send(reinterpret_cast<uint8_t*>(ka.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));
        chlsS[0].send(reinterpret_cast<uint8_t*>(ks.get()),sizeof(PSI::DPF::DPFKeyEarlyTerminal_ByArrayList));

        std::unique_ptr<PSI::DPF::DPFResponseList> ResponseS = std::make_unique<PSI::DPF::DPFResponseList>();
        std::unique_ptr<PSI::DPF::DPFResponseList> ResponseA = std::make_unique<PSI::DPF::DPFResponseList>();
        chlsS[0].recv(reinterpret_cast<uint8_t*>(ResponseS.get()),sizeof(PSI::DPF::DPFResponseList));
        chlsA[0].recv(reinterpret_cast<uint8_t*>(ResponseA.get()),sizeof(PSI::DPF::DPFResponseList));

        DictGen(std::move(ResponseS),std::move(ResponseA));
        InsectionCheck(std::move(oprf_value),input);
        clocks.setpoint("Finish");
        clocks.setDurationEnd("online");
        clocks.printTimePointRecord();
        clocks.printDurationRecord();
        size_t DPFCom = 0 ;
        for(auto chl : chlsS){
            DPFCom += chl.getTotalDataSent() + chl.getTotalDataRecv() ;
            chl.close();
        }

        for(auto chl : chlsA){
            DPFCom += chl.getTotalDataSent() + chl.getTotalDataRecv();
            chl.close();
        }

        std::cout << "DPF com" << DPFCom / 1024.0/1024.0 <<std::endl;
        std::cout << "GC com" << GCOPRFCom / 1024.0/1024.0 <<std::endl;
        std::cout << "Client com" << (GCOPRFCom+DPFCom) / 1024.0/1024.0 <<std::endl;

        sessionS.stop();
        sessionA.stop();
        iosS.stop();
        iosA.stop();
    }
    } // namespace Client
    
} // namespace PSI
