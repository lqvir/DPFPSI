#include "utils.h"
using namespace std;

namespace PSI{
    namespace util{
        void xor_buffers(unsigned char *buf1, const unsigned char *buf2, size_t count){

            for (; count >= 4; count -= 4, buf1 += 4, buf2 += 4) {
                *reinterpret_cast<uint32_t *>(buf1) ^= *reinterpret_cast<const uint32_t *>(buf2);
            }
            for (; count; count--, buf1++, buf2++) {
                *buf1 = static_cast<unsigned char>(*buf1 ^ *buf2);
            }
        }


        void copy_bytes(const void *src, size_t count, void *dst){

            if (!count) {
                return;
            }
            if (!src) {
                throw invalid_argument("cannot copy data: source is null");
            }
            if (!dst) {
                throw invalid_argument("cannot copy data: destination is null");
            }
            copy_n(
                reinterpret_cast<const unsigned char *>(src),
                count,
                reinterpret_cast<unsigned char *>(dst));
        }

        bool compare_bytes(const void *first, const void *second, std::size_t count){
            
            if (!first || !second) {
                throw invalid_argument("cannot compare data: input is null");
            }

            auto first_begin = reinterpret_cast<const unsigned char *>(first);
            auto first_end = first_begin + count;
            auto second_begin = reinterpret_cast<const unsigned char *>(second);

            return equal(first_begin, first_end, second_begin);
        }
        void printchar(char* in,size_t len){
            for(size_t i = 0; i < len; i++){
                printf("%02x",in[i]);
            }
            printf("\n");
        }

        void printchar(unsigned char* in,size_t len){
            for(size_t i = 0; i < len; i++){
                printf("%02x",in[i]);
            }
            printf("\n");
        }

        int blake2b512(void* in,size_t inlen,void* out,size_t out_len){
            uint8_t md[EVP_MAX_MD_SIZE];
            uint32_t md_size;
            EVP_MD_CTX* ctx = EVP_MD_CTX_new();
            EVP_Digest(in,inlen,md,&md_size,EVP_sha256(),NULL);
            if(out_len > md_size){
                return -1;
            }
            // printchar((uint8_t*) md, md_size);

            memcpy(out,md,out_len);
            EVP_MD_CTX_free(ctx);
            return 0;
        }
    } //namespace util

}  // namespace PSI
