#pragma once
#include <iostream>
#include <vector>
#include "gsl/span"
#include "dhoprf_common.h"
#include "openssl/rand.h"
// #include "ecpoint_openssl.h"
#include "psi/common/item.h"

namespace PSI{
    namespace OPRF{

        class OPRFSender {
        public:
            OPRFSender() = default;
            ~OPRFSender();

            // Prepare OPRF key and ECGROUP
            int init();
            OPRFValue ComputeItemHash(const Item &item);
            std::vector<OPRFValue> ComputeHashes(const gsl::span<const Item> &oprf_item);
            std::vector<std::string> ProcessQeries(const std::vector<std::string> &quries);
            

            // static std::vector<unsigned char> ProcessQueries(
            //     gsl::span<const unsigned char> oprf_queries, const OPRFKey &oprf_key);

            // static std::pair<HashedItem, LabelKey> GetItemHash(
            //     const Item &item, const OPRFKey &oprf_key);

            // static std::vector<HashedItem> ComputeHashes(
            //     const gsl::span<const Item> &oprf_items, const OPRFKey &oprf_key);

            // static std::vector<std::pair<HashedItem, EncryptedLabel>> ComputeHashes(
            //     const gsl::span<const std::pair<Item, Label>> &oprf_item_labels,
            //     const OPRFKey &oprf_key,
            //     std::size_t label_byte_count,
            //     std::size_t nonce_byte_count);
            
        private:
            EC_GROUP* curve;
            BIGNUM* oprf_key;
            BN_CTX* ctx_b;
            BN_CTX* ctx_ecc;
            size_t point_hex_len;
            size_t BN_BYTE_LEN,POINT_BYTE_LEN,POINT_COMPRESSED_BYTE_LEN;
        }; // class OPRFSender


    }
}

void test_func();