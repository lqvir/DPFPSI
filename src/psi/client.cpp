#include "client.h"
#include "cuckoo_hash/cuckoo.h"
#include <unordered_set>
namespace PSI
{
    namespace Client
    {
        std::vector<std::string> PSIClient::OPRFQuery(const std::vector<Item>& input){
            DHOPRFReceiver.init();
            auto OPRFQuery = DHOPRFReceiver.process_items(input);
            return OPRFQuery;
        }
        std::vector<std::string> PSIClient::OPRFResponse(const std::vector<std::string>& response){
            auto output = DHOPRFReceiver.process_response(response);
            return output;
        }
        void PSIClient::Cuckoo_All_location(const std::vector<std::string>& oprf_input){
            std::vector<kuku::LocFunc> hash_funcs;
            for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
                hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
            }
            std::unordered_set<kuku::location_type> IndexSets;
                std::cout << "oprf_value" <<std::endl;;
            
            for(auto x : oprf_input){
                util::printchar((uint8_t*)x.data(),x.size());
                for (auto &hf : hash_funcs) {
                    IndexSets.emplace(hf(kuku::make_item((uint8_t*)x.substr(0,16).data())));
                }
            }
            std::cout << __LINE__ << std::endl;
                for(auto it = IndexSets.begin(); it != IndexSets.end(); it++){
                    std::cout << *it << std::endl;
                }
            IndexSets_by_block.resize(cuckoo::block_num);
            for(auto it = IndexSets.begin();it!=IndexSets.end();it++){
                IndexSets_by_block.at(cuckoo::get_block_id(*it)).emplace_back(cuckoo::get_pos(*it));
            }
            for(auto &Indexset: IndexSets_by_block){
                if(Indexset.size()>= cuckoo::max_set_size){
                    throw std::runtime_error("The number of elements in the set exceeds the maximum value.");
                }
                // std::cout << std::endl;

                // for(auto x : Indexset){
                //     std::cout << x << std::endl;
                // }
                // std::cout << std::endl;
            }


        }
        
    void PSIClient::DPFGen(DPF::DPFKeyList& Ks,DPF::DPFKeyList& Ka){
        DPFServer.init();

        for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            auto block_size = IndexSets_by_block.at(block_id).size();
            for(size_t pos_id = 0; pos_id < block_size; pos_id++){
           
                DPFServer.Gen(IndexSets_by_block.at(block_id).at(pos_id),1,Ks.at(block_id).at(pos_id),Ka.at(block_id).at(pos_id));
            }
            for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
                Ka.at(block_id).at(pad_idx).RandomKey();
                Ks.at(block_id).at(pad_idx).RandomKey();
            }
        }
    }
    void PSIClient::DPFGen(
                    DPF::DPFKeyEarlyTerminalList& Ks,
                    DPF::DPFKeyEarlyTerminalList& Ka)
    {
        DPFServer.init();

        for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            auto block_size = IndexSets_by_block.at(block_id).size();
            for(size_t pos_id = 0; pos_id < block_size; pos_id++){
           
                DPFServer.Gen(IndexSets_by_block.at(block_id).at(pos_id),1,Ks.at(block_id).at(pos_id),Ka.at(block_id).at(pos_id));
            }
            for(size_t pad_idx = block_size ;pad_idx < cuckoo::max_set_size; pad_idx++){
                Ka.at(block_id).at(pad_idx).RandomKey();
                Ks.at(block_id).at(pad_idx).RandomKey();
            }
        }
    
    }

    void PSIClient::DictGen(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA){
        
        
        for(size_t block_id =0 ; block_id < cuckoo::block_num ; block_id ++ ){
            auto& DPFResponseS = ResponseListFromS[block_id];
            auto& DPFResponseA = ResponseListFromA[block_id];
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
    void PSIClient::InsectionCheck(const std::vector<std::string>& oprf_input,const std::vector<Item>& input){
        std::vector<kuku::LocFunc> hash_funcs;
        std::string zero_mask;
        zero_mask.assign(Leading_zero_length,'\0');
        for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
            hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
        }
        std::vector<std::pair<Item,PSI::Label>> Ins;
        for(size_t idx = 0; idx < oprf_input.size(); idx++){
            auto &x = oprf_input[idx];

            auto random_mask = x.substr(16,Leading_zero_length+Label_byte_size);
            for (auto &hf : hash_funcs) {
                auto binID = hf(kuku::make_item((uint8_t*)x.substr(0,16).data()));
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
#if LogLevel <= 1
                    util::printchar((uint8_t*)XorLabelMask.value().data(),XorLabelMask.value().size());
#endif
                    
                    if(0 == memcmp(XorLabelMask.value().data(),ZeroMask,Leading_zero_length)){
                        Ins.emplace_back(input.at(idx),toLabel(XorLabelMask));// 暂时
                        break;
                    } 
                }
            }
        }
        for(auto x : Ins){
            printf("item:\n");
            util::printchar(x.first.get_as<uint8_t>().data(),x.first.get_as<uint8_t>().size());
            printf("label:\n");
            util::printchar(x.second.get_as<uint8_t>().data(),x.second.get_as<uint8_t>().size());
        }
        std::cout << "Insection Size" << Ins.size() << std::endl;

    }
    
     

    } // namespace Client
    
} // namespace PSI
