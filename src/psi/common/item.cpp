#include "item.h"
#include <cstring>
namespace PSI{
    std::string Item::to_string() const
    {
        return util::to_string(get_as<uint32_t>());
    }

    LabelMask xor_LabelMask(const LabelMask& buf1,const LabelMask& buf2){
        auto buf1_ = buf1.get_as<uint32_t>();
        auto buf2_  = buf2.get_as<uint32_t>();
        LabelMask Result;
        const auto &res = Result.get_as<uint32_t>();
        for(size_t idx = 0; idx < (Mask_byte_size>>2); idx++){
            res[idx] = buf1_[idx]^ buf2_[idx];
        }
        return Result;
    }

    Label toLabel(const LabelMask& buf){
        return Label(buf.get_as<uint8_t>().subspan(Leading_zero_length,Label_byte_size));
    }
}
