#pragma once

#include "kuku/kuku.h"
#include <vector>
#include <memory>
#include "psi/param.h"
namespace PSI
{
    namespace cuckoo
    {


        
        
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
