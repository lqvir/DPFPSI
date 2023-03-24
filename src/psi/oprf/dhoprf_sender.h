#pragma once
#include <iostream>
#include <vector>
// #include "gsl/span"
#include "dhoprf_common.h"
#include "openssl/rand.h"
// #include "ecpoint_openssl.h"
#include "psi/common/item.h"
#include <span>
namespace PSI{
    namespace OPRF{

        class OPRFSender {
        public:
            OPRFSender() = default;
            ~OPRFSender();

            // Prepare OPRF key and ECGROUP
            int init();
            OPRFValue ComputeItemHash(const Item &item,BN_CTX* ctx = NULL);
            std::vector<OPRFValue> ComputeHashes(const std::span<const Item> &oprf_item);
            std::vector<std::string> ProcessQueries(const std::vector<std::string> &queries);
            std::vector<std::string> ProcessQueriesThread(const std::vector<std::string> &queries);
            

        private:
            EC_GROUP* curve;
            BIGNUM* oprf_key;
            BN_CTX* ctx_b;
            size_t point_hex_len;
            size_t BN_BYTE_LEN,POINT_BYTE_LEN,POINT_COMPRESSED_BYTE_LEN;
        }; // class OPRFSender


    }
}

void test_func();