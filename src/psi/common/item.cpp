#include "item.h"
#include <cstring>
namespace PSI{
    std::string Item::to_string() const
    {
        return util::to_string(get_as<uint32_t>());
    }

    LabelMask xor_LabelMask(const LabelMask& buf1,const LabelMask& buf2){
        const auto len = Label_byte_size + Leading_zero_length;
        auto buf1_ = buf1.get_as<uint32_t>();
        auto buf2_  = buf2.get_as<uint32_t>();
        uint32_t res[len>>2];
        for(size_t idx = 0; idx < (len>>2); idx++){
            res[idx] = buf1_[idx]^ buf2_[idx];
        }
        LabelMask Result(std::span(res,len));
        return Result;
    }

    Label toLabel(const LabelMask& buf){
        return Label(buf.get_as<uint8_t>().subspan(Leading_zero_length,Label_byte_size));
    }
}
