#pragma once 
#include "psi/param.h"
#include <vector>
#include <array>
#include <span>
namespace PSI{
    void Bit_Set1_onebyte(uint8_t& byte_input,size_t idx);
    void Bit_Set0_onebyte(uint8_t& byte_input,size_t idx);
    void Bit_xor1_onebyte(uint8_t& byte_input,size_t idx);

    void Bit_set1_bytes(uint8_t* input,size_t idx);
    void Bit_set0_bytes(uint8_t* input,size_t idx);
    void Bit_xor1_bytes(uint8_t* input,size_t idx);


    bool bitcheck(const uint8_t* input,size_t idx);
    
} // namespace PSI
