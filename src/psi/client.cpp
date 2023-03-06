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
                std::cout << std::endl;

                for(auto x : Indexset){
                    std::cout << x << std::endl;
                }
                std::cout << std::endl;
            }


        }
        
    void PSIClient::DPFGen(){
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

    void PSIClient::DPFConstruct(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA){
        for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            auto& DPFResponseS = ResponseListFromS[block_id];
            auto& DPFResponseA = ResponseListFromA[block_id];

            for(size_t KeyID = 0; KeyID < cuckoo::max_set_size ; KeyID++){
                auto& DPFValueListS =  DPFResponseS.DPFValue[KeyID];
                auto& DPFValueListA =  DPFResponseA.DPFValue[KeyID];

                for(size_t ValueID = 0; ValueID < cuckoo::block_size; ValueID++){
                    auto temp = DPFServer.reconstruct(DPFValueListA[ValueID],DPFValueListS[ValueID]);
                    // printf("block_id %d KeyID %d ValueID %d : %02x \n",block_id,KeyID,ValueID,temp);
                }

            }
            // std::cout << "cuckoo table S " << std::endl;
            // for(auto x : DPFResponseS.cuckoo_table){
            //     util::printchar((uint8_t*)x.data(),leading_zero_length+Label_byte_size);
            // }
            // std::cout << "cuckoo table A " << std::endl;
            // for(auto x : DPFResponseA.cuckoo_table){
            //     util::printchar((uint8_t*)x.data(),leading_zero_length+Label_byte_size);
            // }
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
                auto& DPFValueListS =  DPFResponseS.DPFValue[index_id];
                auto& DPFValueListA =  DPFResponseA.DPFValue[index_id];
                auto DPFValue = DPFServer.reconstruct(DPFValueListA[index],DPFValueListS[index]);
                if((DPFValue&1)){
                    Dict.insert(std::make_pair(cuckoo::get_BinID(index,block_id),DPFResponseS.cuckoo_table.at(index)));

                    printf("block_id %d KeyID %d ValueID %d : %02x \n",block_id,index_id,index,DPFValue);

                }  
            }
        }

    }
    void PSIClient::InsectionCheck(const std::vector<std::string>& oprf_input,const std::vector<Item>& input){
        std::vector<kuku::LocFunc> hash_funcs;
        std::string zero_mask;
        zero_mask.assign(leading_zero_length,'\0');
        for (uint32_t i = 0; i < cuckoo::hash_func_count; i++) {
            hash_funcs.emplace_back(cuckoo::table_size, kuku::make_item(i, 0));
        }
        std::vector<std::pair<Item,std::string>> Ins;
        for(size_t idx = 0; idx < oprf_input.size(); idx++){
            auto &x = oprf_input[idx];

            auto random_mask = x.substr(16,leading_zero_length+Label_byte_size);
            for (auto &hf : hash_funcs) {
                auto binID = hf(kuku::make_item((uint8_t*)x.substr(0,16).data()));
                auto search = Dict.find(binID);
                if(search != Dict.end()){
                    std::string ans = search->second;
                    // std::cout << (ans == search->second) << std::endl;
                    // util::printchar((uint8_t*)ans.data(),ans.size());
                    // util::printchar((uint8_t*)search->second.data(),search->second.size());
                    // util::printchar((uint8_t*)random_mask.data(),random_mask.size());
                    util::xor_buffers((uint8_t*)ans.data(),(uint8_t*)random_mask.data(),leading_zero_length+Label_byte_size);
                    util::printchar((uint8_t*)ans.data(),ans.size());
                    if(zero_mask == ans.substr(0,leading_zero_length)){
                        Ins.emplace_back(input.at(idx),ans.substr(leading_zero_length,Label_byte_size));
                        break;
                    } 
                }
            }
        }
        for(auto x : Ins){
            printf("item:\n");
            util::printchar(x.first.get_as<uint8_t>().data(),x.first.get_as<uint8_t>().size());
            printf("label:\n");
            util::printchar(x.second.data(),x.second.size());
        }

    }
    
     

    } // namespace Client
    
} // namespace PSI
