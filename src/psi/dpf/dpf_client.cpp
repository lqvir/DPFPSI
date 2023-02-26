#include "dpf_client.h"

namespace PSI
{
    namespace DPF
    {
        
        uint8_t dpf_client::Eval(std::bitset<DPF_INPUT_BIT_SIZE> x_bitset,uint8_t party_number){
            std::bitset<Lambda> share_i = key.share;
            
            // std::cout<<x_bitset<<std::endl;
            for(size_t i = DPF_INPUT_BIT_SIZE - 1; i > 0 ; i--){
                std::bitset<Lambda> Hs_out;
                Keyed_hash_func(share_i,Hs_out);
                
                if(x_bitset.test(i)){
                    Hs_out ^= share_i;
                }
                if(share_i.test(0)){
                    Hs_out ^= key.cw[i-1];
                }
                share_i = Hs_out;
                // std::cout << "i:" << i << std::endl;
                // std::cout << share_i << std::endl;
            }
            
            std::bitset<Lambda> high_low;
            std::bitset<Lambda> temp = share_i;
            temp[0]=share_i[0] ^x_bitset[0];
            Keyed_hash_func(temp,high_low);
            // std::cout << "high low " << std::endl << high_low << std::endl;
            std::bitset<Lambda> share_n = high_low;
            if(share_i.test(0)){
                auto mid_string = key.cw_n.to_string();
                if(x_bitset.test(0)){
                    mid_string.erase(mid_string.size()-2,1);
                }
                else{
                    mid_string.pop_back();
                }
                // std::cout << std::bitset<128>(mid_string) << std::endl;

                share_n ^= std::bitset<128>(mid_string);
            }
            // std::cout << share_n << std::endl;
            if(party_number){
                return (uint8_t)((uint8_t)(-1)*(share_n[2]+share_n[0]*key.cw_n_plus_1));
            }
            else{
                return (uint8_t)((share_n[2]+share_n[0]*key.cw_n_plus_1));
            }

        }

    } // namespace DPF
    
} // namespace PSI
