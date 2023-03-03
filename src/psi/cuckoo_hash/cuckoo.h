#include "kuku/kuku.h"
#include <vector>
#include <memory>
#include "psi/param.h"
namespace PSI
{
    namespace cuckoo
    {

        constexpr size_t hash_func_count = 3;
        constexpr size_t table_size  = 16;
        constexpr size_t cuckoo_table_insert_attempts = 1000;
        constexpr size_t block_size = 8;
        constexpr size_t block_num = table_size/block_size;

        inline size_t get_bin_id(size_t pos){
            return pos%block_size;
        }
        inline size_t get_block_id(size_t pos){
            return pos/block_size;
        }
        inline size_t get_pos(size_t BinID,size_t BlockID){
            return BlockID*block_size+BinID;
        }

      

    } // namespace cuckoo
    
} // namespace PSI
