#pragma once
#include <iostream>
#include "psi/common/item.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "psi/cuckoo_hash/cuckoo.h"
#include <bitset>
namespace PSI
{
    namespace DPF
    {
        
        const uint8_t AESKEY[16] = {0xeb,0x28,0xcc,0xa6,0x5c,0x64,0x48,0xa1,0xf9,0x98,0x02,0xe8,0x68,0x0c,0x70,0xfd};
      
        using DPFResponse = std::array<LabelMask,cuckoo::max_set_size>;
        
        using share_type = std::array<uint8_t,Lambda_bytes>;
        struct DPFKeyEarlyTerminal_ByArray{

            share_type share;
            std::array<share_type,DPF_EAYLY_HIGH - 1> cw;
            share_type cw_n_HCW;
            uint8_t cw_n_last;
            std::array<uint8_t,DPF_COMPRESS_NODES_BYTE_NUMBER> cw_n1;
            
            DPFKeyEarlyTerminal_ByArray(){
                memset(share.data(),0,share.size());
                memset(cw.data(),0,cw.size());
                memset(cw_n_HCW.data(),0,cw_n_HCW.size());
                memset(cw_n1.data(),0,cw_n1.size());

                cw_n_last = 0;
            }
            inline void RandomKey();

        };
        
        inline void DPFKeyEarlyTerminal_ByArray::RandomKey(){
            RAND_bytes(share.data(),share.size());
            for(auto x : cw){
                RAND_bytes(x.data(),x.size());
            }
            RAND_bytes(&cw_n_last,1);
            RAND_bytes(cw_n_HCW.data(),cw_n_HCW.size());
            RAND_bytes(cw_n1.data(),cw_n1.size());
        }
        typedef  std::array<std::array<DPF::DPFKeyEarlyTerminal_ByArray,cuckoo::max_set_size>,cuckoo::block_num> DPFKeyEarlyTerminal_ByArrayList; 
        
        
        
        struct DPFKey
        {
            /* data */
            std::bitset<Lambda>  share;
            std::array<std::bitset<Lambda>,DPF_INPUT_BIT_SIZE - 1> cw;
            std::bitset<Lambda+1> cw_n;
            uint8_t cw_n_plus_1;
            DPFKey(){
                this->share = 0;
                for(auto x : cw){
                    x = 0;
                }
                cw_n = 0;
                cw_n_plus_1 = 0;
            }
            inline void RandomKey();
        };

        inline void DPFKey::RandomKey(){
            const size_t all_len = Lambda_bytes*(DPF_INPUT_BIT_SIZE+1)+2;
            uint8_t buffer[all_len];
            RAND_bytes(buffer,all_len);
            std::stringstream ss;
            for(auto x : buffer){
                ss << std::bitset<8>(x);
            }
            ss >> this->share;
            for(size_t idx = 0; idx < DPF_INPUT_BIT_SIZE - 1; idx++){
                ss >> this->cw.at(idx);
            }
            ss >> cw_n;
            this->cw_n_plus_1 = buffer[all_len-1];
        }
        
        typedef  std::array<std::array<DPF::DPFKey,cuckoo::max_set_size>,cuckoo::block_num> DPFKeyList; 
        typedef std::array<DPFResponse ,cuckoo::block_num> DPFResponseList;
        // typedef std::array<uint8_t,((size_t)1<<DPF_INPUT_BIT_SIZE)> pcGGMLeafList;

       
        struct DPFKeyEarlyTerminal
        {
            /* data */
            std::bitset<Lambda>  share;
            std::array<std::bitset<Lambda>,DPF_EAYLY_HIGH - 1> cw;
            std::bitset<Lambda+1> cw_n;
            std::bitset<DPF_COMPRESS_NODES_NUMBER> cw_n_1;
            DPFKeyEarlyTerminal(){
                share = 0;
                for(auto x : cw){
                    x = 0;
                }
                cw_n = 0;
                cw_n_1 = 0;
            }
            inline void RandomKey();
        };

        inline void DPFKeyEarlyTerminal::RandomKey(){
            const size_t all_len = Lambda_bytes*(DPF_EARLY_BIT_SIZE+2)+2;
            uint8_t buffer[all_len];
            RAND_bytes(buffer,all_len);
            std::stringstream ss;
            for(auto x : buffer){
                ss << std::bitset<8>(x);
            }
            ss >> this->share;
            for(size_t idx = 0; idx < DPF_EAYLY_HIGH - 1; idx++){
                ss >> this->cw.at(idx);
            }
            ss >> cw_n;
            ss >> cw_n_1;
        }


        typedef  std::array<std::array<DPF::DPFKeyEarlyTerminal,cuckoo::max_set_size>,cuckoo::block_num> DPFKeyEarlyTerminalList; 

        typedef std::bitset<((size_t)1 << DPF_INPUT_BIT_SIZE)> pcGGMLeafList;


        inline void sigma(uint8_t* input){
            uint64_t* in_u64 =(uint64_t*) input;
            in_u64[1] ^= in_u64[0];
            std::swap(in_u64[0],in_u64[1]);
        }

        inline std::pair<uint64_t,uint64_t> sigma(uint64_t lw, uint64_t rw){
            return std::make_pair(lw^rw,lw);
        }
        inline Item sigma(const Item& in){
            return Item(in.get_as<uint64_t>()[0]^in.get_as<uint64_t>()[1],in.get_as<uint64_t>()[0]);
        }
        inline void to_Bitset(std::bitset<Lambda>& out, uint8_t* in){
            uint64_t* temp = (uint64_t*) in;
            std::stringstream ss;
            ss << std::bitset<64>(temp[0]).to_string() << std::bitset<64>(temp[1]).to_string();
            ss >> out;
        }

        inline void Random_Bitset(std::bitset<Lambda>& in){
            uint64_t temp[2];
            RAND_bytes((uint8_t*)temp,16);
            std::stringstream ss;
            ss << std::bitset<64>(temp[0]).to_string() << std::bitset<64>(temp[1]).to_string();
            ss >> in;
        }

        inline void Keyed_hash_func(const Item& input,Item& output){
            auto sigma_x = sigma(input);
            auto ctx = EVP_CIPHER_CTX_new();
            EVP_CIPHER_CTX_init(ctx);
            int ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, AESKEY, NULL);
            EVP_CIPHER_CTX_set_padding(ctx, 0);
            int mlen = 0;
            uint8_t* out = output.get_as<uint8_t>().data();

            // uint8_t out[32];
            ret = EVP_EncryptUpdate(ctx, (unsigned char*)out, &mlen, (const unsigned char*)sigma_x.get_as<uint8_t>().data(), sigma_x.get_as<uint8_t>().size());
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);
        
            util::xor_buffers(out,sigma_x.get_as<uint8_t>().data(),Lambda_bytes);
        }

        inline void Keyed_hash_func(const uint8_t* input,uint8_t* output){
            uint8_t input_u8[Lambda_bytes];
            memcpy(input_u8,input,Lambda_bytes);
            sigma(input_u8);
            auto ctx = EVP_CIPHER_CTX_new();
            EVP_CIPHER_CTX_init(ctx);
            int ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, AESKEY, NULL);
            EVP_CIPHER_CTX_set_padding(ctx, 0);
            int mlen = 0;
            // uint8_t out[32];
            ret = EVP_EncryptUpdate(ctx, (unsigned char*)output, &mlen, input_u8, Lambda_bytes);
            EVP_CIPHER_CTX_cleanup(ctx);
            EVP_CIPHER_CTX_free(ctx);

            PSI::util::xor_buffers(output,input_u8,Lambda_bytes);
        }

        inline void Keyed_hash_func(const std::bitset<Lambda>& in,std::bitset<Lambda>& out){
            uint8_t outChar[Lambda_bytes];
            uint64_t inChar[Lambda_bytes>>3] ={0};
            auto str = in.to_string();
            // std::cout << str << std::endl;
            // std::cout << str.substr(0,64) << std::endl;
            // std::cout << str.substr(64,64) << std::endl;

            inChar[0] = std::stoull(str.substr(0,64),NULL,2);
            inChar[1] = std::stoull(str.substr(64,64),NULL,2);

            Keyed_hash_func((uint8_t*)inChar,outChar);
            to_Bitset(out,outChar);
        }


        inline void Convert_to_G(const std::bitset<Lambda>& in,uint8_t& out){
            if(in[1] == 1){
                out = 1;
            }
            else{
                out = 0;
            }
        }

        inline void Convert_to_G(const std::bitset<Lambda>&in, std::bitset<DPF_COMPRESS_NODES_NUMBER>& out){
            // std::cout << in.to_string() << std::endl;
            out =std::bitset<DPF_COMPRESS_NODES_NUMBER>(in.to_string().substr(Lambda-DPF_COMPRESS_NODES_NUMBER-1,DPF_COMPRESS_NODES_NUMBER));
            // std::cout << out.to_string() << std::endl;
        }
        inline void Convert_to_G(const share_type& in,uint8_t *out){
            util::copy_bytes(in.data()+1,DPF_COMPRESS_NODES_BYTE_NUMBER,out);
        }

        
    
    

    
    } // namespace DPF
    
} // namespace PSI
