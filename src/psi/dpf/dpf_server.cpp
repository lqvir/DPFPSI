
#include "dpf_server.h"
#include "pcGGM.h"
#include "openssl/rand.h"
#include "bitset"
#include "psi/common/BitOpreate.h"
namespace PSI
{
    namespace DPF
    {
 

        
        void dpf_server::Gen(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1){
            #if LogLevel == 0
            std::cout << alpha << std::endl;
            util::printchar(reinterpret_cast<uint8_t*>(&alpha),8);
            #endif
            
            
            
            auto alpha_uint8 = reinterpret_cast<uint8_t*>(&alpha);
            uint8_t Delta[Lambda_bytes];
            RAND_bytes(Delta,Lambda_bytes);
            Bit_set1_bytes(Delta,0);

            RAND_bytes(key0.share.data(),key0.share.size());
            util::xor_buffers(key1.share.data(),key0.share.data(),Delta,Lambda_bytes);
            
            auto ctx = EVP_CIPHER_CTX_new();
            EVP_CIPHER_CTX_init(ctx);
            int ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, AESKEY, NULL);
            EVP_CIPHER_CTX_set_padding(ctx, 0);


            share_type share_i_0,share_i_1;
            share_i_0 = key0.share;
            share_i_1 = key1.share;
            {
                share_type CW_i;
                share_type share_next0,share_next1;

                for(size_t i = DPF_INPUT_BIT_SIZE - 1; i > DPF_EARLY_BIT_SIZE; i--){
                    share_type Hs_out_1,Hs_out_0;
                    Keyed_hash_func(share_i_0.data(),Hs_out_0.data(),ctx);
                    Keyed_hash_func(share_i_1.data(),Hs_out_1.data(),ctx);

                    util::xor_buffers(CW_i.data(),Hs_out_0.data(),Hs_out_1.data(),Lambda_bytes);
                    

                    if(bitcheck(alpha_uint8,i)){
                        util::xor_buffers(share_next0.data(),share_i_0.data(),Hs_out_0.data(),Lambda_bytes);
                        util::xor_buffers(share_next1.data(),share_i_1.data(),Hs_out_1.data(),Lambda_bytes);
                    }
                    else{
                        util::xor_buffers(CW_i.data(),Delta,Lambda_bytes);
                        share_next0 = Hs_out_0;
                        share_next1 = Hs_out_1;
                    }
                    if(bitcheck(share_i_0.data(),0)){
                        util::xor_buffers(share_next0.data(),CW_i.data(),Lambda_bytes);
                    }
                    if(bitcheck(share_i_1.data(),0)){
                        util::xor_buffers(share_next1.data(),CW_i.data(),Lambda_bytes);
                    }

                    key0.cw[i-1- DPF_EARLY_BIT_SIZE] = CW_i;
                    key1.cw[i-1- DPF_EARLY_BIT_SIZE] = CW_i;
                    share_i_0 = share_next0;
                    share_i_1 = share_next1;
                }

            }
            share_type share_i_0_xor_1;
            share_type share_i_1_xor_1;

            share_type High_Low_super0_sub0,High_Low_super1_sub0,High_Low_super0_sub1,High_Low_super1_sub1;

            share_i_0_xor_1 = share_i_0;
            share_i_1_xor_1 = share_i_1;
            Bit_xor1_bytes(share_i_0_xor_1.data(),0);
            Bit_xor1_bytes(share_i_1_xor_1.data(),0);

            Keyed_hash_func(share_i_0.data(),High_Low_super0_sub0.data(),ctx);
            Keyed_hash_func(share_i_0_xor_1.data(),High_Low_super1_sub0.data(),ctx);
            Keyed_hash_func(share_i_1.data(),High_Low_super0_sub1.data(),ctx);  
            Keyed_hash_func(share_i_1_xor_1.data(),High_Low_super1_sub1.data(),ctx);

            share_type& HCW = key0.cw_n_HCW;
            if(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE)){
                util::xor_buffers(HCW.data(),High_Low_super0_sub0.data(),High_Low_super0_sub1.data(),Lambda_bytes);
            }else{
                util::xor_buffers(HCW.data(),High_Low_super1_sub0.data(),High_Low_super1_sub1.data(),Lambda_bytes);
            }
            if(bitcheck(High_Low_super0_sub0.data(),0)^bitcheck(High_Low_super0_sub1.data(),0)^(!bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE))){
                Bit_set1_bytes(HCW.data(),0);
            }
            else{
                Bit_set0_bytes(HCW.data(),0);
            }
            auto &LCW = key0.cw_n_last;
            LCW = HCW[0];
            if(bitcheck(High_Low_super1_sub0.data(),0)^bitcheck(High_Low_super1_sub1.data(),0)^(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE))){
                Bit_Set1_onebyte(LCW,0);
            }
            else{
                Bit_Set0_onebyte(LCW,0);
            }
            key1.cw_n_HCW = key0.cw_n_HCW;
            key1.cw_n_last = key0.cw_n_last;
            share_type share_n_0,share_n_1,HCW_LCW;
            HCW_LCW  = HCW;
            if(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE)){
                share_n_0 = High_Low_super1_sub0;
                share_n_1 = High_Low_super1_sub1;
                HCW_LCW[0] = LCW; 
            }else{
                share_n_0 = High_Low_super0_sub0;
                share_n_1 = High_Low_super0_sub1;
            }
            if(bitcheck(share_i_0.data(),0)){
                util::xor_buffers(share_n_0.data(),HCW_LCW.data(),Lambda_bytes);
            }
            if(bitcheck(share_i_1.data(),0)){
                util::xor_buffers(share_n_1.data(),HCW_LCW.data(),Lambda_bytes);
            }
            std::array<uint8_t,DPF_COMPRESS_NODES_BYTE_NUMBER> vec_beta,sn_list0,sn_list1;
            memset(vec_beta.data(),0,sizeof(vec_beta));
    

            Bit_set1_bytes(vec_beta.data(),alpha&((1<<DPF_EARLY_BIT_SIZE)-1));

            Convert_to_G(share_n_0,sn_list0.data());
            Convert_to_G(share_n_1,sn_list1.data());
            util::xor_buffers(sn_list0.data(),sn_list1.data(),DPF_COMPRESS_NODES_BYTE_NUMBER);
            util::xor_buffers(key0.cw_n1.data(),sn_list0.data(),vec_beta.data(),DPF_COMPRESS_NODES_BYTE_NUMBER);
            key1.cw_n1 = key0.cw_n1;
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);
        }


        void dpf_server::Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta,DPFKey& key0,DPFKey& key1){
                        // step 2
            std::bitset<Lambda> Delta;

            Random_Bitset(Delta);

            Delta[0] = 1;
            uint8_t s0_t0[Lambda_bytes];
            RAND_bytes(s0_t0,Lambda_bytes);
            // step 3 
            Random_Bitset(key0.share);
            key1.share = key0.share ^ Delta;
        
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
            // uint8_t temp = 0;
            // temp = ((uint8_t)(share_n_0[0]-share_n_1[0]))*((uint8_t)(share_n_0[1]-share_n_1[1])+beta);
       

            if(share_n_0[0] == share_n_1[0]){
                throw std::runtime_error("an error in the pcGGM calculation");
            }


            // std::cout << (int)key0.cw_n_plus_1 << ' ' <<  (int)temp << std::endl;
            key0.cw_n_plus_1 = ((uint8_t)(share_n_0[1]^share_n_1[1])^beta);
           
            key1.cw_n_plus_1 = key0.cw_n_plus_1;

            // std::cout << "key without early terminal:" <<(share_n_0[0]^share_n_1[0])<< std::endl;
            // std::cout << (int)key1.cw_n_plus_1 << std::endl;
        }

        void dpf_server::Gen(std::bitset<DPF_INPUT_BIT_SIZE> alpha_bitset,uint8_t beta,DPFKeyEarlyTerminal& key0,DPFKeyEarlyTerminal& key1){
            std::bitset<Lambda> Delta;
            
            Random_Bitset(Delta);
            Delta[0] = 1;
            uint8_t s0_t0[Lambda_bytes];
            RAND_bytes(s0_t0,Lambda_bytes);
            // step 3 
            Random_Bitset(key0.share);
            key1.share = key0.share ^ Delta;

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
                for(size_t i = DPF_INPUT_BIT_SIZE - 1  ; i > DPF_EARLY_BIT_SIZE ; i--){

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

                    key0.cw[i-1 - DPF_EARLY_BIT_SIZE] = CW_i;
                    key1.cw[i-1 - DPF_EARLY_BIT_SIZE] = CW_i;
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

            // step 10
            std::string HCW,LCW0,LCW1;
            if(alpha_bitset.test(DPF_EARLY_BIT_SIZE)){
                HCW = (High_Low_super0_sub0^High_Low_super0_sub1).to_string();
            }else{
                HCW = (High_Low_super1_sub0^High_Low_super1_sub1).to_string();
            }
            HCW.pop_back();
            // step 11

            if(High_Low_super0_sub0[0]^High_Low_super0_sub1[0]^(~alpha_bitset[DPF_EARLY_BIT_SIZE])){
                LCW0 = "1";
            }
            else{
                LCW0 = "0";
            }
            if(High_Low_super1_sub0[0]^High_Low_super1_sub1[0]^alpha_bitset[DPF_EARLY_BIT_SIZE]){
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
            if(alpha_bitset.test(DPF_EARLY_BIT_SIZE)){
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
            // Early Terminal  Conversion
            std::bitset<DPF_COMPRESS_NODES_NUMBER> vec_beta,sn_list0,sn_list1;
            vec_beta[alpha_bitset.to_ullong()%(1<<DPF_EARLY_BIT_SIZE)] = 1;

            if(share_n_0[0] == share_n_1[0]){
                throw std::runtime_error("an error in the pcGGM calculation");
            }


            Convert_to_G(share_n_0,sn_list0);
            Convert_to_G(share_n_1,sn_list1);
            key0.cw_n_1 = (sn_list0^sn_list1^vec_beta);
            key1.cw_n_1 = key0.cw_n_1;

            // std::cout << "key:" <<(share_n_0[0]^share_n_1[0])<< std::endl;
            // for(size_t idx = 0 ; idx < DPF_COMPRESS_NODES_NUMBER; idx ++){
            //     std::cout << idx <<sn_list0[idx] <<' ' << key0.cw_n_1[idx] << vec_beta[idx] << std::endl;
            // }
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
