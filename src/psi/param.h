#pragma once
#include <vector>
#include <string>
namespace PSI
{
    constexpr size_t Label_bit_size = 128;
    constexpr size_t Label_byte_size = 16;

    constexpr size_t Leading_zero_length =  8 ;
    constexpr size_t Item_byte_size = 16;
    constexpr size_t Mask_byte_size = Label_byte_size + Leading_zero_length;

    constexpr uint8_t ZeroMask[Leading_zero_length] = {0};
    #define LogLevel 1

    namespace DPF{
        constexpr size_t DPF_INPUT_BIT_SIZE = 12;
        constexpr size_t DPF_INPUT_BYTE_SIZE = 2;
        constexpr size_t Lambda = 128 ;
        constexpr size_t Lambda_bytes = 16;

        // DPF_EARLY_BIT_SIZE = min(DPF_INPUT_BIT_SIZE - ceil(lambda/log2(2)),DPF_INPUT_BIT_SIZE)
        constexpr size_t DPF_EARLY_BIT_SIZE = 6;
        constexpr size_t DPF_EAYLY_HIGH = DPF_INPUT_BIT_SIZE - DPF_EARLY_BIT_SIZE;
        // 2 ^ (DPF_INPUT_BIT_SIZE - DPF_EARLY_BIT_SIZE)
        constexpr size_t DPF_COMPRESS_NODES_NUMBER = 64;
        constexpr size_t DPF_COMPRESS_NODES_BYTE_NUMBER = DPF_COMPRESS_NODES_NUMBER>>3;
       
    } // namespace DPF

    constexpr uint8_t bit_mask[8] = {
        0b00000001,
        0b00000010,
        0b00000100,
        0b00001000,
        0b00010000,
        0b00100000,
        0b01000000,
        0b10000000
    };

    constexpr uint8_t bit_mask_reverse[8] = {
        0b11111110,
        0b11111101,
        0b11111011,
        0b11110111,
        0b11101111,
        0b11011111,
        0b10111111,
        0b01111111
    };
    

    namespace cuckoo{
        namespace cuckooparam1048576{
            constexpr size_t hash_func_count = 3;
            constexpr size_t table_size  = 1048576+1024*256;
            constexpr size_t cuckoo_table_insert_attempts = 1000;
            constexpr size_t block_size = 4096;
            constexpr size_t block_num = table_size/block_size;
            constexpr size_t max_set_size = 80;
        }        
        namespace cuckooparam65536{
            constexpr size_t hash_func_count = 3;
            constexpr size_t table_size  = 65536+256*50;
            constexpr size_t cuckoo_table_insert_attempts = 1000;
            constexpr size_t block_size = 256;
            constexpr size_t block_num = table_size/block_size;
            constexpr size_t max_set_size = 80;
        }
        using namespace cuckooparam1048576;

    } // namespace cuckoo
    
} // namespace PSI
