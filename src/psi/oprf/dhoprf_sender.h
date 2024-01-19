#pragma once
#include <iostream>
#include <vector>
// #include "gsl/span"
#include "dhoprf_common.h"
#include "openssl/rand.h"
// #include "ecpoint_openssl.h"
#include "psi/common/item.h"
#include <span>
#include "psi/fourq/FourQ.h"
#include "ecpoint_openssl.h"
namespace PSI{
    namespace OPRF{

        class OPRFSender {
        public:
            OPRFSender() = default;
            ~OPRFSender();

            // Prepare OPRF key and ECGROUP
            int init();
            OPRFValueOpenssL ComputeItemHash(const Item &item,BN_CTX* ctx = NULL);
            std::vector<OPRFValueOpenssL> ComputeHashes(const std::span<const Item> &oprf_item);
            std::vector<OPRFPoint> ProcessQueries(const std::vector<OPRFPoint> &queries);
            std::vector<OPRFPoint> ProcessQueriesThread(const std::vector<OPRFPoint> &queries);
            OPRFValueOpenssL ComputeItemHashFourQ(const Item &item);
            std::vector<OPRFValueOpenssL> ComputeHashesFourQ(const std::span<const Item> &oprf_items);
            std::vector<OPRFPointFourQ> 
                ProcessQueriesFourQ(std::vector<OPRFPointFourQ> &queries);
            std::vector<OPRFPointFourQ> 
                ProcessQueriesThreadFourQ(std::vector<OPRFPointFourQ> &queries);

        private:
            EC_GROUP* curve;
            BIGNUM* oprf_key;
            uint8_t oprf_key_fourq[32];

            BN_CTX* ctx_b;
            size_t point_hex_len;
            size_t BN_BYTE_LEN,POINT_BYTE_LEN;
        }; // class OPRFSender


    }
}

void test_func();