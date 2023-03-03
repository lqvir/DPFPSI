#include "server.h"
#include "kuku/kuku.h"
#include "cuckoo_hash/cuckoo.h"
namespace PSI
{
    namespace Server
    {
        std::vector<std::string> PSIServer::init(const std::vector<Item>& input,const std::vector<Label>& input_Label){
            DHOPRFSender.init();
            auto output = DHOPRFSender.ComputeHashes(input);
            

            kuku::KukuTable cuckoo_table(
                cuckoo::table_size,      // Size of the hash table
                0,                                      // Not using a stash
                cuckoo::hash_func_count, // Number of hash functions
                { 0, 0 },                               // Hardcoded { 0, 0 } as the seed
                cuckoo::cuckoo_table_insert_attempts,           // The number of insertion attempts
                { 0, 0 });
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++){
                const auto &item = output[item_idx].substr(0,16);
                if (!cuckoo_table.insert(kuku::make_item((uint8_t*)item.data()))) {
                    // Insertion can fail for two reasons:
                    //
                    //     (1) The item was already in the table, in which case the "leftover
                    //     item" is empty; (2) Cuckoo hashing failed due to too small table or
                    //     too few hash functions.
                    //
                    // In case (1) simply move on to the next item and log this issue. Case (2)
                    // is a critical issue so we throw and exception.
                    if (cuckoo_table.is_empty_item(cuckoo_table.leftover_item())) {
                        
                        std::cout <<"Skipping repeated insertion of items["
                            << item_idx <<"]:" ;
                        util::printchar((uint8_t*)item.data(),16);
                    } else {
                         std::cout <<
                            "Failed to insert items["
                            << item_idx << "] "
                            << " cuckoo table fill-rate: " << cuckoo_table.fill_rate();
                        throw std::runtime_error("failed to insert item into cuckoo table");
                    }
                }

            }

            std::vector<std::string> hash_table(cuckoo::table_size);
            for (size_t item_idx = 0; item_idx < server_set_size_; item_idx++) {
                const auto &item = output[item_idx].substr(0,16);

                auto item_loc = cuckoo_table.query(kuku::make_item((uint8_t*)item.data())).location();


                hash_table[item_loc] = output[item_idx].substr(16,leading_zero_length+Label_byte_size);
                // std::cout << item_loc << std::endl;

                util::xor_buffers((uint8_t*)(hash_table[item_loc].data()+leading_zero_length),(uint8_t*)input_Label[item_idx].data(),Label_byte_size);
                // itt.table_idx_to_item_idx_[item_loc.location()] = item_idx;
            }

            // for(size_t idx = 0 ; idx < cuckoo::table_size ; idx ++){
            //     std::cout << idx;
            //     util::printchar((uint8_t*)hash_table[idx].data(),leading_zero_length+Label_byte_size);
            // }
            

            return hash_table;

        }

        std::vector<std::string>  PSIServer::process_query(const std::vector<std::string>& input){
            return DHOPRFSender.ProcessQeries(input);
        }

    } // namespace Server
    
} // namespace PSI
