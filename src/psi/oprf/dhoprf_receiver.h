#pragma once

#include <iostream>
#include <vector>

// OpenSSL

#include "openssl/ec.h"
// PSI
#include "dhoprf_common.h"
#include "psi/common/utils.h"
#include "psi/common/item.h"
#include <span>
//GSL
// #include "gsl/span"

namespace PSI{
    namespace OPRF{
        class OPRFReceiver{
        public:
            OPRFReceiver() = default;
            ~OPRFReceiver();

            int init();
            std::vector<std::string> process_items(std::span<const Item> oprf_items);
            std::vector<std::string> process_items_threads(std::span<const Item> oprf_items);
            std::vector<std::string> process_response(const std::vector<std::string> responses);
            std::vector<OPRFValue> process_response_threads(const std::vector<std::string> responses);
        private:
            EC_GROUP* curve;
            BN_CTX* ctx_b;
            BN_CTX* ctx_ecc;
            const BIGNUM* order;
            size_t point_hex_len;
            std::vector<BIGNUM*> key_inv;
            size_t BN_BYTE_LEN,POINT_BYTE_LEN,POINT_COMPRESSED_BYTE_LEN;

        };



    }
}
