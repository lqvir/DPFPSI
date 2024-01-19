#pragma once

#include <iostream>
#include <vector>

// OpenSSL

#include "openssl/ec.h"
// PSI
#include "dhoprf_common.h"
#include "psi/common/utils.h"
#include "psi/common/item.h"
#include "ecpoint_openssl.h"
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
            std::vector<OPRFPoint> process_items(std::span<const Item> oprf_items);
            std::vector<OPRFPoint> process_items_threads(std::span<const Item> oprf_items);
            std::vector<OPRFValueOpenssL> process_response(const std::vector<OPRFPoint>& responses);
            std::vector<OPRFValueOpenssL> process_response_threads(const std::vector<OPRFPoint>& responses);
        
            std::vector<OPRFPointFourQ> 
                process_itemsFourQ(std::span<const Item> oprf_items);
            std::vector<OPRFPointFourQ> 
                process_items_threadsFourQ(std::span<const Item> oprf_items);
            std::vector<OPRFValueOpenssL> 
                process_responseFourQ(const std::vector<OPRFPointFourQ>& responses);
            std::vector<OPRFValueOpenssL> 
                process_response_threadsFourQ(const std::vector<OPRFPointFourQ>& responses);
        
        private:
            EC_GROUP* curve;
            BN_CTX* ctx_b;
            BN_CTX* ctx_ecc;
            const BIGNUM* order;
            size_t point_hex_len;
            std::vector<BIGNUM*> key_inv;
            std::vector<std::array<uint8_t,32>> key_inv_fourq;
            size_t BN_BYTE_LEN,POINT_BYTE_LEN;

        };



    }
}
