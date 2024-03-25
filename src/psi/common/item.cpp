#include "item.h"
#include <cstring>
namespace PSI{

   


    

    LabelMask xor_LabelMask(const LabelMask& buf1,const LabelMask& buf2){
        auto buf1_ = buf1.get_as<uint64_t>();
        auto buf2_  = buf2.get_as<uint64_t>();
        LabelMask Result;
        const auto &res = Result.get_as<uint64_t>();
        for(size_t idx = 0; idx < (Mask_byte_size>>3); idx++){
            res[idx] = buf1_[idx]^ buf2_[idx];
        }
        // util::xor_buffers(res,buf1_,buf2_,Mask_byte_size);
        return Result;
    }

    Label toLabel(const LabelMask& buf){
        if(Label_byte_size == 0){return Label();} 
        return Label(buf.get_as<uint8_t>().subspan(Leading_zero_length,Label_byte_size));
    }





    

}
