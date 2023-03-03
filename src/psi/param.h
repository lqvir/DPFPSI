#pragma once
#include <vector>

namespace PSI
{
    constexpr size_t Label_bit_size = 64;
    constexpr size_t Label_byte_size = 8;

    constexpr size_t leading_zero_length =  8 ;


    using Label = std::vector<uint8_t>;



} // namespace PSI
