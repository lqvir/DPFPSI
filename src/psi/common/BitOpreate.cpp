#include "BitOpreate.h"
#include <array>
#include <vector>
#include <memory>
namespace PSI
{
    void Bit_Set1_onebyte(uint8_t& byte_input,size_t idx){
        byte_input |= bit_mask[idx];
    }
    void Bit_Set0_onebyte(uint8_t& byte_input,size_t idx){
        byte_input &= bit_mask_reverse[idx];
    }
    void Bit_xor1_onebyte(uint8_t& byte_input,size_t idx){
        byte_input ^= bit_mask[idx];
    }
    void Bit_set1_bytes(uint8_t* input,size_t idx){
        auto byte_idx = idx >> 3;
        auto bit_idx = idx & 0x7;
        Bit_Set1_onebyte(input[byte_idx],bit_idx);
    }
    void Bit_set0_bytes(uint8_t* input,size_t idx){
        auto byte_idx = idx >> 3;
        auto bit_idx = idx & 0x7;
        Bit_Set0_onebyte(input[byte_idx],bit_idx);
    }
    void Bit_xor1_bytes(uint8_t* input,size_t idx){

        auto byte_idx = idx >> 3;
        auto bit_idx = idx & 0x7;
        Bit_xor1_onebyte(input[byte_idx],bit_idx);
      
    }

    bool bitcheck(const uint8_t* input,size_t idx){
        size_t byte_idx = idx >> 3;
        size_t bit_idx = idx & 0x7;
        return (bool)(input[byte_idx]&bit_mask[bit_idx]);
    }


} // namespace PSI
