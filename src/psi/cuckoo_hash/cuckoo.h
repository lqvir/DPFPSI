#pragma once

#include "kuku/kuku.h"
#include <vector>
#include <memory>
#include "psi/param.h"
namespace PSI
{
    namespace cuckoo
    {

        constexpr size_t hash_func_count = 3;
        constexpr size_t table_size  = 16000;
        constexpr size_t cuckoo_table_insert_attempts = 1000;
        constexpr size_t block_size = 320;
        constexpr size_t block_num = table_size/block_size;
        constexpr size_t max_set_size = 160;
        
        
        inline size_t get_pos(size_t BinID){
            return BinID%block_size;
        }
        inline size_t get_block_id(size_t BinID){
            return BinID/block_size;
        }
        inline size_t get_BinID(size_t pos,size_t BlockID){
            return BlockID*block_size+pos;
        }

      

    } // namespace cuckoo
    
} // namespace PSI
