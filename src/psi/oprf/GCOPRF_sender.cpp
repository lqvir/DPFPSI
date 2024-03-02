#include "psi/oprf/GCOPRF_sender.h"


#include <droidCrypto/ChannelWrapper.h>
#include <droidCrypto/gc/circuits/LowMCCircuit.h>
#include <thread>
#include <assert.h>
#include <endian.h>
#include <droidCrypto/utils/Log.h>
#include "psi/common/thread_pool_mgr.h"
#include "psi/common/utils.h"

extern "C" {
    #include <droidCrypto/lowmc/lowmc_pars.h>
    #include <droidCrypto/lowmc/io.h>
    #include <droidCrypto/lowmc/lowmc.h>
    #include <droidCrypto/lowmc/lowmc_128_128_192.h>
}




namespace PSI
{
    namespace GCOPRF
    {

        OPRFSender::OPRFSender(droidCrypto::ChannelWrapper& chan) :  channel_(chan), circ_(chan) {};

        std::unique_ptr<std::vector<GCOPRFValue>> OPRFSender::setup(std::unique_ptr<std::vector<droidCrypto::block>> elements){
            auto time0 = std::chrono::high_resolution_clock::now();
            size_t num_server_elements = elements->size();
            size_t num_threads_ = ThreadPoolMgr::GetThreadCount();

            // MT-bounds
            size_t elements_per_thread = num_server_elements /num_threads_;
            // LOWMC encryption
            // get a random key
            droidCrypto::PRNG::getTestPRNG().get(lowmc_key_.data(), lowmc_key_.size());

            const lowmc_t* params =  droidCrypto::SIMDLowMCCircuitPhases::params;
            lowmc_key_t* key = mzd_local_init(1, params->k);
            mzd_from_char_array(key, lowmc_key_.data(), (params->k)/8);
            expanded_key key_calc = lowmc_expand_key(params, key);

            std::vector<std::thread> threads;
            for(size_t thrd = 0; thrd < num_threads_-1; thrd++) {
                auto t = std::thread([params, key_calc, &elements, elements_per_thread,idx=thrd]{
                    lowmc_key_t* pt = mzd_local_init(1, params->n);
                    for(size_t i = idx*elements_per_thread; i < (idx+1)*elements_per_thread; i++) {
                        mzd_from_char_array(pt, (uint8_t *) (&(*elements)[i]), params->n / 8);
                        mzd_local_t *ct = lowmc_call(params, key_calc, pt);
                        mzd_to_char_array((uint8_t *) (&(*elements)[i]), ct, params->n / 8);
                        mzd_local_free(ct);
                    }
                    mzd_local_free(pt);
                });
                threads.emplace_back(std::move(t));
            }
            lowmc_key_t* pt = mzd_local_init(1, params->n);
            for(size_t i = (num_threads_-1)*elements_per_thread; i < num_server_elements; i++) {
                mzd_from_char_array(pt, (uint8_t *) (&(*elements)[i]), params->n / 8);
                mzd_local_t *ct = lowmc_call(params, key_calc, pt);
                mzd_to_char_array((uint8_t *) (&(*elements)[i]), ct, (params->n) / 8);
                mzd_local_free(ct);
            }
            mzd_local_free(pt);
            for(size_t thrd = 0; thrd < num_threads_ -1; thrd++) {
                threads[thrd].join();
            }
            auto output = std::make_unique<std::vector<GCOPRFValue>>();
            output->resize(num_server_elements);
            for(size_t ele = 0; ele < num_server_elements; ele++){
                util::blake2b512(&(*elements)[ele],16,&(*output)[ele],OPRFValueBytes);

                // memcpy(&(*output)[ele],&(*elements)[ele],16);
            }

            auto time1 = std::chrono::high_resolution_clock::now();
            return std::move(output);
        }

        void OPRFSender::base(){
            size_t num_client_elements;
            channel_.recv((uint8_t*)&num_client_elements, sizeof(num_client_elements));
            num_client_elements = be64toh(num_client_elements);

            droidCrypto::BitVector key_bits(lowmc_key_.data(),
                                            droidCrypto::SIMDLowMCCircuitPhases::params->n);
            circ_.garbleBase(key_bits, num_client_elements);
        }
        void OPRFSender::Online(){
            circ_.garbleOnline();
        
        }

        
    } // namespace GCOPRF
    
} // namespace PSI
