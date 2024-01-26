#include "psi/oprf/GCOPRF_receiver.h"

#include <droidCrypto/ChannelWrapper.h>
#include <droidCrypto/BitVector.h>
#include <droidCrypto/utils/Log.h>
#include <memory>
namespace PSI
{
    namespace GCOPRF
    {

        OPRFReciver::OPRFReciver(droidCrypto::ChannelWrapper& chan) :  channel_(chan), circ_(chan) {}

        void OPRFReciver::base(size_t num_elements){

            size_t num_client_elements = htobe64(num_elements);
            channel_.send((uint8_t*)&num_client_elements, sizeof(num_client_elements));
            circ_.evaluateBase(num_elements);

        }



        void OPRFReciver::Online
            (std::unique_ptr<std::vector<droidCrypto::block>> elements){

            size_t num_client_elements = elements->size();
            
            //do GC evaluation
            
            std::vector<droidCrypto::BitVector> bit_elements;
            bit_elements.reserve(elements->size());
            for(size_t i = 0; i < elements->size(); i++) {
                auto res = *elements.get();

                droidCrypto::BitVector bitinput((uint8_t*)(&res[i]), 128);
                bit_elements.push_back(bitinput);
            }
            channel_.clearStats();
            std::vector<droidCrypto::BitVector> result = circ_.evaluateOnline(bit_elements);


        }
    } // namespace GCOPRF
    
} // namespace PSI
