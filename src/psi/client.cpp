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
            for(auto x : oprf_input){
                std::unordered_set<kuku::location_type> result;
                for (auto &hf : hash_funcs) {
                    result.emplace(hf(kuku::make_item((uint8_t*)x.substr(0,16).data())));
                }
                for(auto it = result.begin(); it != result.end(); it++){
                    std::cout << *it << std::endl;
                }

            }
           

        }


    } // namespace Client
    
} // namespace PSI
