#include "pcGGM.h"

#include "psi/common/BitOpreate.h"


#include "openssl/rand.h"
#include <array>
#include <stack>
namespace PSI{
    namespace DPF{
        namespace pcGGM{
        
        void GenKey(size_t alpha,uint8_t beta,DPFKeyEarlyTerminal_ByArray& key0,DPFKeyEarlyTerminal_ByArray& key1){
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
                    // Keyed_hash_func(share_i_0.data(),Hs_out_0.data(),ctx);
                    // Keyed_hash_func(share_i_1.data(),Hs_out_1.data(),ctx);
                    Keyed_hash_func(share_i_0.data(),Hs_out_0.data());
                    Keyed_hash_func(share_i_1.data(),Hs_out_1.data());
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

            // Keyed_hash_func(share_i_0.data(),High_Low_super0_sub0.data(),ctx);
            // Keyed_hash_func(share_i_0_xor_1.data(),High_Low_super1_sub0.data(),ctx);
            // Keyed_hash_func(share_i_1.data(),High_Low_super0_sub1.data(),ctx);  
            // Keyed_hash_func(share_i_1_xor_1.data(),High_Low_super1_sub1.data(),ctx);
            Keyed_hash_func(share_i_0.data(),High_Low_super0_sub0.data());
            Keyed_hash_func(share_i_0_xor_1.data(),High_Low_super1_sub0.data());
            Keyed_hash_func(share_i_1.data(),High_Low_super0_sub1.data());  
            Keyed_hash_func(share_i_1_xor_1.data(),High_Low_super1_sub1.data());
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


        void GenKey(size_t alpha,uint8_t beta,DPFKey& key0,DPFKey& key1){
#if LogLevel == 0
            std::cout << alpha << std::endl;
            util::printchar(reinterpret_cast<uint8_t*>(&alpha),8);
#endif
            
            auto alpha_uint8 = reinterpret_cast<uint8_t*>(&alpha);
            
            dr::SecureRandom prg;
            dr::block delta = prg.randBlock() | LSB1Block;
            key0.share = prg.randBlock();


            key1.share = delta ^ key0.share;
            
            dr::block share_i_0,share_i_1;
            share_i_0 = key0.share;
            share_i_1 = key1.share;
            
            {
                dr::block CW_i;
                dr::block share_next0,share_next1;

                for(size_t i = DPF_INPUT_BIT_SIZE - 1; i > DPF_EARLY_BIT_SIZE; i--){
                    dr::block Hs_out_1,Hs_out_0;
                    // Keyed_hash_func(share_i_0.data(),Hs_out_0.data(),ctx);
                    // Keyed_hash_func(share_i_1.data(),Hs_out_1.data(),ctx);

                    Keyed_hash_func(share_i_0,Hs_out_0);

                    Keyed_hash_func(share_i_1,Hs_out_1);
                    // util::xor_buffers(CW_i.data(),Hs_out_0.data(),Hs_out_1.data(),Lambda_bytes);
                    CW_i = Hs_out_0 ^ Hs_out_1;

                    if(bitcheck(alpha_uint8,i)){
                        
                        share_next0 = share_i_0 ^ Hs_out_0;
                        share_next1 = share_i_1 ^ Hs_out_1;
                        // util::xor_buffers(share_next0.data(),share_i_0.data(),Hs_out_0.data(),Lambda_bytes);
                        // util::xor_buffers(share_next1.data(),share_i_1.data(),Hs_out_1.data(),Lambda_bytes);
                    }
                    else{
                        CW_i = CW_i ^ delta;
                        // util::xor_buffers(CW_i.data(),Delta,Lambda_bytes);
                        share_next0 = Hs_out_0;
                        share_next1 = Hs_out_1;
                    }

                    uint8_t* share_i_0u8 = ((uint8_t*)&share_i_0);
                    uint8_t* share_i_1u8 = ((uint8_t*)&share_i_1);

                    if((*share_i_0u8)&1){
                        share_next0 = share_next0 ^ CW_i;
                    }
                    if((*share_i_1u8)&1){
                        share_next1 = share_next1 ^ CW_i;
                    }

                    key0.cw[i-1- DPF_EARLY_BIT_SIZE] = CW_i;
                    key1.cw[i-1- DPF_EARLY_BIT_SIZE] = CW_i;
                    share_i_0 = share_next0;
                    share_i_1 = share_next1;
                }

            }
            dr::block share_i_0_xor_1;
            dr::block share_i_1_xor_1;

            dr::block High_Low_super0_sub0,High_Low_super1_sub0,High_Low_super0_sub1,High_Low_super1_sub1;

            share_i_0_xor_1 = share_i_0;
            share_i_1_xor_1 = share_i_1;

            share_i_0_xor_1 ^= LSB1Block;
            share_i_1_xor_1 ^= LSB1Block;

            // Bit_xor1_bytes((uint8_t*)&share_i_0_xor_1,0);
            // Bit_xor1_bytes((uint8_t*)&share_i_0_xor_1,0);

    
            Keyed_hash_func(share_i_0,High_Low_super0_sub0);
            Keyed_hash_func(share_i_0_xor_1,High_Low_super1_sub0);
            Keyed_hash_func(share_i_1,High_Low_super0_sub1);  
            Keyed_hash_func(share_i_1_xor_1,High_Low_super1_sub1);
            dr::block& HCW = key0.cw_n_HCW;
            if(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE)){
                HCW = High_Low_super0_sub0 ^ High_Low_super0_sub1;
            }else{
                HCW = High_Low_super1_sub0 ^ High_Low_super1_sub1;
            }
            if(bitcheck((uint8_t*)&High_Low_super0_sub0,0)^bitcheck((uint8_t*)&High_Low_super0_sub1,0)^(!bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE))){
                Bit_set1_bytes((uint8_t*)&HCW,0);
            }
            else{
                Bit_set0_bytes((uint8_t*)&HCW,0);
            }
            auto &LCW = key0.cw_n_last;
            LCW = HCW[0];
            if(bitcheck((uint8_t*)&High_Low_super1_sub0,0)^bitcheck((uint8_t*)&High_Low_super1_sub1,0)^(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE))){
                Bit_Set1_onebyte(LCW,0);
            }
            else{
                Bit_Set0_onebyte(LCW,0);
            }
            key1.cw_n_HCW = key0.cw_n_HCW;
            key1.cw_n_last = key0.cw_n_last;
            dr::block share_n_0,share_n_1,HCW_LCW;
            HCW_LCW  = HCW;
            if(bitcheck(alpha_uint8,DPF_EARLY_BIT_SIZE)){
                share_n_0 = High_Low_super1_sub0;
                share_n_1 = High_Low_super1_sub1;
                ((uint8_t*)&HCW_LCW)[0] = LCW; 
            }else{
                share_n_0 = High_Low_super0_sub0;
                share_n_1 = High_Low_super0_sub1;
            }
            if(bitcheck((uint8_t*)&share_i_0,0)){
                share_n_0 = HCW_LCW ^ share_n_0;
                // util::xor_buffers(share_n_0.data(),HCW_LCW.data(),Lambda_bytes);
            }
            if(bitcheck((uint8_t*)&share_i_1,0)){
                share_n_1 = share_n_1 ^ HCW_LCW;
                // util::xor_buffers(share_n_1.data(),HCW_LCW.data(),Lambda_bytes);
            }
            std::array<uint8_t,DPF_COMPRESS_NODES_BYTE_NUMBER> vec_beta,sn_list0,sn_list1;
            
            memset(vec_beta.data(),0,sizeof(vec_beta));
    

            Bit_set1_bytes(vec_beta.data(),alpha&((1<<DPF_EARLY_BIT_SIZE)-1));

            Convert_to_G((uint8_t*)&share_n_0,sn_list0.data());
            Convert_to_G((uint8_t*)&share_n_1,sn_list1.data());
            util::xor_buffers(sn_list0.data(),sn_list1.data(),DPF_COMPRESS_NODES_BYTE_NUMBER);
            util::xor_buffers(key0.cw_n1.data(),sn_list0.data(),vec_beta.data(),DPF_COMPRESS_NODES_BYTE_NUMBER);
            key1.cw_n1 = key0.cw_n1;
        
        }

        pcGGMLeafList GenTree(const DPFKey& key){
            pcGGMLeafList ans;
            size_t cnt = 0;
            std::stack<std::pair<dr::block,size_t>> DFSStack;
            auto mid_string0 = key.cw_n_HCW;
            // PSI::util::printchar((uint8_t*)&mid_string0,16);
            auto mid_string1 = key.cw_n_HCW;
            // PSI::util::printchar((uint8_t*)&mid_string0,16);


            *(uint8_t*)&mid_string1 = key.cw_n_last; 
            DFSStack.push(std::make_pair(key.share,0));
            // PSI::util::printchar((uint8_t*)&mid_string1,16);
            
            while (!DFSStack.empty())
            {
                auto now = DFSStack.top();
                DFSStack.pop();
                if(now.second == DPF_EAYLY_HIGH - 1){ // n - 1 break the loop 
                    dr::block share_n0 = dr::ZeroBlock;
                    dr::block share_n1;
                    // Keyed_hash_func(now.first.data(),share_n0.data(),ctx);
                    Keyed_hash_func(now.first,share_n0);
                    dr::block now_first = now.first ^ LSB1Block;
                    // Keyed_hash_func(now_first.data(),share_n1.data(),ctx);
                    
                    Keyed_hash_func(now_first,share_n1);
                    if(bitcheck((uint8_t*)&now.first,0)){
                        share_n0 ^= mid_string0;
                        share_n1 ^= mid_string1;
                    }
                    uint8_t G_items[DPF_COMPRESS_NODES_BYTE_NUMBER] = {0};
                    bool flag = bitcheck((uint8_t*)&share_n0,0);

                    Convert_to_G((uint8_t*)&share_n0,G_items);
                    for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                        ans[cnt++] =  bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    }
                    if(cnt >= cuckoo::block_size){
                        break;
                    }
                    flag = bitcheck((uint8_t*)&share_n1,0);
                    Convert_to_G((uint8_t*)&share_n1,G_items);
                    for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                        ans[cnt++]=  bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    }
                    if(cnt >= cuckoo::block_size){
                        break;
                    }

                }
                else{
                    dr::block Hs_out = dr::ZeroBlock;
                    // memset(Hs_out.data(),0,sizeof(Hs_out));
                    // Keyed_hash_func(now.first.data(),Hs_out.data(),ctx);
                    Keyed_hash_func(now.first,Hs_out);
                    if(bitcheck((uint8_t*)&now.first,0)){
                        Hs_out=Hs_out^key.cw.at(DPF_EAYLY_HIGH-2-now.second);
                    }
                    dr::block xor_out = Hs_out^now.first;
                    DFSStack.push(std::make_pair(xor_out,now.second+1));
                    DFSStack.push(std::make_pair(Hs_out,now.second+1));
                }

            }

            return ans;

        }

        void GenTree(const DPFKey& key, uint64_t* ans){
            size_t cnt = 0;
            std::stack<std::pair<dr::block,size_t>> DFSStack;
            auto mid_string0 = key.cw_n_HCW;
            // PSI::util::printchar((uint8_t*)&mid_string0,16);
            auto mid_string1 = key.cw_n_HCW;
            // PSI::util::printchar((uint8_t*)&mid_string0,16);


            *(uint8_t*)&mid_string1 = key.cw_n_last; 
            DFSStack.push(std::make_pair(key.share,0));
            // PSI::util::printchar((uint8_t*)&mid_string1,16);
            
            while (!DFSStack.empty())
            {
                auto now = DFSStack.top();
                DFSStack.pop();
                if(now.second == DPF_EAYLY_HIGH - 1){ // n - 1 break the loop 
                    dr::block share_n0 = dr::ZeroBlock;
                    dr::block share_n1;
                    // Keyed_hash_func(now.first.data(),share_n0.data(),ctx);
                    Keyed_hash_func(now.first,share_n0);
                    dr::block now_first = now.first ^ LSB1Block;
                    // Keyed_hash_func(now_first.data(),share_n1.data(),ctx);
                    
                    Keyed_hash_func(now_first,share_n1);
                    if(bitcheck((uint8_t*)&now.first,0)){
                        share_n0 ^= mid_string0;
                        share_n1 ^= mid_string1;
                    }
                    // uint8_t G_items[DPF_COMPRESS_NODES_BYTE_NUMBER] = {0};
                    // bool flag = bitcheck((uint8_t*)&share_n0,0);

                    // Convert_to_G((uint8_t*)&share_n0,G_items);
                    // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                    //     ans[cnt++] =  bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    // }
                    // if(cnt >= cuckoo::block_size){
                    //     break;
                    // }
                    // flag = bitcheck((uint8_t*)&share_n1,0);
                    // Convert_to_G((uint8_t*)&share_n1,G_items);
                    // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                    //     ans[cnt++]=  bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    // }
                    // if(cnt >= cuckoo::block_size){
                    //     break;
                    // }


                    uint64_t G_items = 0;
                    uint64_t cw_n1 = (*(uint64_t*)key.cw_n1.data());
                    Convert_to_G((uint8_t*)&share_n0,(uint8_t*)&G_items);
                    uint64_t flag = (uint64_t)bitcheck((uint8_t*)&share_n0,0) * UINT64_MAX;
                    // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                    //     ans[pos] =  bitcheck((uint8_t*)&G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                        
                    // }
                    ans[cnt++] =  G_items ^ (cw_n1 & flag); 
                    if(cnt*64 >= cuckoo::block_size){
                        break;
                    }
                    Convert_to_G((uint8_t*)&share_n1,(uint8_t*)&G_items);
                    flag = (uint64_t)bitcheck((uint8_t*)&share_n1,0) * UINT64_MAX;

                    // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                    //     ans[pos]=  bitcheck((uint8_t*)&G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    // }
                    ans[cnt++] = G_items ^ (cw_n1 & flag);

                    if(cnt*64 >= cuckoo::block_size){
                        break;
                    }
                }
                else{
                    dr::block Hs_out = dr::ZeroBlock;
                    // memset(Hs_out.data(),0,sizeof(Hs_out));
                    // Keyed_hash_func(now.first.data(),Hs_out.data(),ctx);
                    Keyed_hash_func(now.first,Hs_out);
                    if(bitcheck((uint8_t*)&now.first,0)){
                        Hs_out=Hs_out^key.cw.at(DPF_EAYLY_HIGH-2-now.second);
                    }
                    dr::block xor_out = Hs_out^now.first;
                    DFSStack.push(std::make_pair(xor_out,now.second+1));
                    DFSStack.push(std::make_pair(Hs_out,now.second+1));
                }

            }


        }


        pcGGMLeafList GenTree(const DPFKeyEarlyTerminal_ByArray key){

            pcGGMLeafList ans;
            size_t cnt = 0;
            std::stack<std::pair<share_type,size_t>> DFSStack;
            auto mid_string0 = key.cw_n_HCW;
            auto mid_string1 = key.cw_n_HCW;
            mid_string1[0] = key.cw_n_last; 
            DFSStack.push(std::make_pair(key.share,0));
            while (!DFSStack.empty())
            {
                auto now = DFSStack.top();
                DFSStack.pop();
                if(now.second == DPF_EAYLY_HIGH - 1){ // n - 1 break the loop 
                    share_type share_n0;
                    share_type share_n1;
                    // Keyed_hash_func(now.first.data(),share_n0.data(),ctx);
                    DPF::Keyed_hash_func(now.first.data(),share_n0.data());
                    share_type now_first = now.first;
                    Bit_xor1_onebyte(now_first[0],0);
                    // Keyed_hash_func(now_first.data(),share_n1.data(),ctx);
                    DPF::Keyed_hash_func(now_first.data(),share_n1.data());
                    if(bitcheck(now.first.data(),0)){
                        util::xor_buffers(share_n0.data(),mid_string0.data(),Lambda_bytes);
                        util::xor_buffers(share_n1.data(),mid_string1.data(),Lambda_bytes);
                    }
                    uint8_t G_items[DPF_COMPRESS_NODES_BYTE_NUMBER] = {0};
                    Convert_to_G(share_n0,G_items);
                    for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                        ans[cnt++] = bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & bitcheck(share_n0.data(),0));
                    }
                    if(cnt >= cuckoo::block_size){
                        break;
                    }
                    Convert_to_G(share_n1,G_items);
                    for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                        ans[cnt++] = bitcheck(G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & bitcheck(share_n1.data(),0));
                    }
                    if(cnt >= cuckoo::block_size){
                        break;
                    }

                }
                else{
                    share_type Hs_out;
                    memset(Hs_out.data(),0,sizeof(Hs_out));
                    // Keyed_hash_func(now.first.data(),Hs_out.data(),ctx);
                    Keyed_hash_func(now.first.data(),Hs_out.data());
                    if(bitcheck(now.first.data(),0)){
                        util::xor_buffers(Hs_out.data(),key.cw.at(DPF_EAYLY_HIGH-2-now.second).data(),Lambda_bytes);
                    }
                    share_type xor_out;
                    util::xor_buffers(xor_out.data(),Hs_out.data(),now.first.data(),Lambda_bytes);
                    DFSStack.push(std::make_pair(xor_out,now.second+1));
                    DFSStack.push(std::make_pair(Hs_out,now.second+1));
                }

            }

     
            return ans;

        }

        void GenTreeRecursionWork(uint64_t* ans, size_t depth, dr::block work,const DPFKey& key, size_t pos,const dr::block& mid_string0,const dr::block& mid_string1){
            if(depth == DPF_EAYLY_HIGH - 1){
                pos *= 2;
                dr::block share_n0 = dr::ZeroBlock;
                dr::block share_n1;
                // Keyed_hash_func(now.first.data(),share_n0.data(),ctx);
                Keyed_hash_func(work,share_n0);
                dr::block work_first = work ^ LSB1Block;
                // Keyed_hash_func(now_first.data(),share_n1.data(),ctx);
                
                Keyed_hash_func(work_first,share_n1);
                if(bitcheck((uint8_t*)&work,0)){
                    share_n0 ^= mid_string0;
                    share_n1 ^= mid_string1;
                }
                uint64_t G_items = 0;
                uint64_t cw_n1 = (*(uint64_t*)key.cw_n1.data());
                Convert_to_G((uint8_t*)&share_n0,(uint8_t*)&G_items);
                uint64_t flag = (uint64_t)bitcheck((uint8_t*)&share_n0,0) * UINT64_MAX;
                // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                //     ans[pos] =  bitcheck((uint8_t*)&G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                    
                // }
                ans[pos] =  G_items ^ (cw_n1 & flag); 
                if(pos*64 >= cuckoo::block_size){
                    return;
                }
                Convert_to_G((uint8_t*)&share_n1,(uint8_t*)&G_items);
                flag = (uint64_t)bitcheck((uint8_t*)&share_n1,0) * UINT64_MAX;

                // for(size_t idx = 0; idx < DPF_COMPRESS_NODES_NUMBER; idx++){
                //     ans[pos]=  bitcheck((uint8_t*)&G_items,idx) ^ (bitcheck((uint8_t*)key.cw_n1.data(),idx) & flag);
                // }
                ans[pos+1] = G_items ^ (cw_n1 & flag);
                

                return;
            }

            dr::block Hs_out = dr::ZeroBlock;
            // // memset(Hs_out.data(),0,sizeof(Hs_out));
            // // Keyed_hash_func(now.first.data(),Hs_out.data(),ctx);
            Keyed_hash_func(work,Hs_out);
            if(bitcheck((uint8_t*)&work,0)){
                Hs_out=Hs_out^key.cw.at(DPF_EAYLY_HIGH-2-depth);
            }
            dr::block xor_out = Hs_out^work;
            GenTreeRecursionWork(ans,depth+1,Hs_out,key,2*pos,mid_string0,mid_string1);
            GenTreeRecursionWork(ans,depth+1,xor_out,key,2*pos+1,mid_string0,mid_string1);

        }


        void GenTreeRecursion(const DPFKey& key, uint64_t* ans){
            size_t pos = 0;
            auto mid_string0 = key.cw_n_HCW;
            auto mid_string1 = key.cw_n_HCW;

            *(uint8_t*)&mid_string1 = key.cw_n_last; 
            GenTreeRecursionWork(ans,0,key.share,key,pos,mid_string0,mid_string1);
        }

        }
    } // namespace DPF
} // namespace PSI
