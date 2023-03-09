
#include "dpf_server.h"
#include "openssl/rand.h"
#include "bitset"
namespace PSI
{
    namespace DPF
    {
 
        void dpf_server::init() 
        {
         


        }
        
        void dpf_server::Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta,DPFKey& key0,DPFKey& key1){
                        // step 2
            Random_Bitset(Delta);

            Delta[0] = 1;
            uint8_t s0_t0[Lambda_bytes];
            RAND_bytes(s0_t0,Lambda_bytes);
            // step 3 
            Random_Bitset(key0.share);
            key1.share = key0.share ^ Delta;
        
            
            
            // std::stringstream ss;
            // std::string alpha_bit;
            // for(size_t i = 0,char temp[16] ; i < alpha.size(); i++){
            //     ss << std::bitset<8>(alpha[i]);
            // }
            // std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset;
            // ss >> alpha_bitset;
            // step 1

            // std::cout << alpha_bitset << std::endl;
            // std::cout << Delta << std::endl;
            std::bitset<Lambda> share_i_0,share_i_1;
            share_i_0 = key0.share;
            share_i_1 = key1.share;
            
            {
                std::bitset<Lambda> CW_i;
                std::bitset<Lambda> share_next_0;
                std::bitset<Lambda> share_next_1;
                    // std::cout << share_i_0 << std::endl;
                    // std::cout << share_i_1 << std::endl;
                // step 4-7
                for(size_t i = DPF_INPUT_BIT_SIZE - 1  ; i > 0 ; i--){

                    std::bitset<Lambda> Hs_out_1;
                    std::bitset<Lambda> Hs_out_0;
                    Keyed_hash_func(share_i_0,Hs_out_0);
                    Keyed_hash_func(share_i_1,Hs_out_1);
                    CW_i = Hs_out_0^ Hs_out_1;
                    
                    if(alpha_bitset.test(i)){
                        share_next_0 = Hs_out_0 ^ share_i_0;
                        share_next_1 = Hs_out_1 ^ share_i_1;
                    }
                    else{
                        CW_i ^= Delta;
                        share_next_0 = Hs_out_0;
                        share_next_1 = Hs_out_1;
                    }
                    if(share_i_0.test(0)){
                        share_next_0 ^= CW_i;
                    }
                    if(share_i_1.test(0)){
                        share_next_1 ^= CW_i;
                    }

                    key0.cw[i-1] = CW_i;
                    key1.cw[i-1] = CW_i;
                    share_i_0 = share_next_0;
                    share_i_1 = share_next_1;
                    // std::cout <<Hs_out_0  << std::endl;
                    // std::cout <<Hs_out_1  << std::endl;

                    // std::cout <<share_next_0  << std::endl;
                    // std::cout <<share_next_1  << std::endl;
                    // std::cout <<(CW_i^Hs_out_0^Hs_out_1)  << std::endl;
                    // std::cout << "i:" << i << std::endl;
                    


                    // std::cout << share_i_0 << std::endl;
                    // std::cout << share_i_1 << std::endl;
                    // if((share_i_0 ^ share_i_1 )== Delta){
                    //     std::cout <<"right" << std::endl;
                    // }
                }
            }
            // step 8-9
            std::bitset<Lambda> High_Low_super0_sub0,High_Low_super1_sub0,High_Low_super0_sub1,High_Low_super1_sub1;
            Keyed_hash_func(share_i_0,High_Low_super0_sub0);
            Keyed_hash_func(share_i_0^std::bitset<128>(1),High_Low_super1_sub0);
            Keyed_hash_func(share_i_1,High_Low_super0_sub1);
            Keyed_hash_func(share_i_1^std::bitset<128>(1),High_Low_super1_sub1);

            // std::cout << "high or low " << std::endl;
            // std::cout << High_Low_super0_sub0 << std::endl;
            // std::cout << High_Low_super1_sub0 << std::endl;
            // std::cout << High_Low_super0_sub1 << std::endl;
            // std::cout << High_Low_super1_sub1 << std::endl;

            // step 10
            std::string HCW,LCW0,LCW1;
            if(alpha_bitset.test(0)){
                HCW = (High_Low_super0_sub0^High_Low_super0_sub1).to_string();
            }else{
                HCW = (High_Low_super1_sub0^High_Low_super1_sub1).to_string();
            }
            HCW.pop_back();
            // step 11

            if(High_Low_super0_sub0[0]^High_Low_super0_sub1[0]^(~alpha_bitset[0])){
                LCW0 = "1";
            }
            else{
                LCW0 = "0";
            }
            if(High_Low_super1_sub0[0]^High_Low_super1_sub1[0]^alpha_bitset[0]){
                LCW1 = "1";
            }
            else{
                LCW1 = "0";
            }
            // step 12
            // std::cout << HCW << std::endl;
            // std::cout << LCW0 << std::endl;
            // std::cout << LCW1 << std::endl;

            key0.cw_n = std::bitset<129>(HCW+LCW0+LCW1);
            key1.cw_n = key0.cw_n;
            // std::cout << key0.cw_n << std::endl;

            std::bitset<128> share_n_0,share_n_1,HCW_LCW;
            if(alpha_bitset.test(0)){
                share_n_0 = High_Low_super1_sub0;
                share_n_1 = High_Low_super1_sub1;
                HCW_LCW = std::bitset<128>(HCW.append(LCW1));
            }else{
                share_n_0 = High_Low_super0_sub0;
                share_n_1 = High_Low_super0_sub1;
                HCW_LCW = std::bitset<128>(HCW.append(LCW0));
            }
            if(share_i_0.test(0)){
                share_n_0 ^= HCW_LCW;
            }
            if(share_i_1.test(0)){
                share_n_1 ^= HCW_LCW;
            }
            // std::cout << HCW_LCW << std::endl;

            // std::cout << share_n_0 << std::endl;
            // std::cout << share_n_1 << std::endl;

            // step 15
            key0.cw_n_plus_1 = ((uint8_t)(share_n_0[0]-share_n_1[0]))*((uint8_t)(share_n_0[2]-share_n_1[2])+beta);
            key1.cw_n_plus_1 = key0.cw_n_plus_1;
        }
        void dpf_server::DPFConstruct(const DPF::DPFResponseList& ResponseListFromS,const DPF::DPFResponseList& ResponseListFromA){
            // for(size_t block_id = 0; block_id < cuckoo::block_num; block_id++){
            //     auto& DPFResponseS = ResponseListFromS[block_id];
            //     auto& DPFResponseA = ResponseListFromA[block_id];

            //     for(size_t KeyID = 0; KeyID < cuckoo::max_set_size ; KeyID++){
            //         auto& DPFValueListS =  DPFResponseS.DPFValue[KeyID];
            //         auto& DPFValueListA =  DPFResponseA.DPFValue[KeyID];

            //         for(size_t ValueID = 0; ValueID < cuckoo::block_size; ValueID++){
            //             auto temp = reconstruct(DPFValueListA[ValueID],DPFValueListS[ValueID]);
            //             // printf("block_id %d KeyID %d ValueID %d : %02x \n",block_id,KeyID,ValueID,temp);
            //         }

            //     }
            //     // std::cout << "cuckoo table S " << std::endl;
            //     // for(auto x : DPFResponseS.cuckoo_table){
            //     //     util::printchar((uint8_t*)x.data(),leading_zero_length+Label_byte_size);
            //     // }
            //     // std::cout << "cuckoo table A " << std::endl;
            //     // for(auto x : DPFResponseA.cuckoo_table){
            //     //     util::printchar((uint8_t*)x.data(),leading_zero_length+Label_byte_size);
            //     // }
            // }
        }
        
    } // namespace DPF
    
} // namespace PSI
