#pragma once
#include <vector>
#include <string>
namespace PSI
{
    constexpr size_t Label_bit_size = 128;
    constexpr size_t Label_byte_size = 16;

    constexpr size_t Leading_zero_length =  8 ;
    constexpr size_t Item_byte_size = 16;

    constexpr uint8_t ZeroMask[Leading_zero_length] = {0};
    #define LogLevel 1

} // namespace PSI
