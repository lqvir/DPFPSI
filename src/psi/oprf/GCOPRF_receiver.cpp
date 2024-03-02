#include "psi/oprf/GCOPRF_receiver.h"

#include <droidCrypto/ChannelWrapper.h>
#include <droidCrypto/BitVector.h>
#include <droidCrypto/utils/Log.h>
#include <memory>

#include "psi/common/utils.h"
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



        std::unique_ptr<std::vector<GCOPRFValue>> OPRFReciver::Online
            (std::unique_ptr<std::vector<droidCrypto::block>> elements){

            size_t num_client_elements = elements->size();
            
            //do GC evaluation
            channel_.clearStats();
            std::vector<droidCrypto::BitVector> bit_elements;
            bit_elements.reserve(elements->size());
            for(size_t i = 0; i < elements->size(); i++) {
                droidCrypto::BitVector bitinput((uint8_t*)(&(*elements)[i]), 128);
                bit_elements.push_back(bitinput);
            }
            

            std::vector<droidCrypto::BitVector> result = circ_.evaluateOnline(bit_elements);
            auto output = std::make_unique<std::vector<GCOPRFValue>>();
            output->resize(num_client_elements);


            for(size_t ele = 0; ele < num_client_elements; ele++){
                util::blake2b512((uint8_t*)result[ele].data(),16,&(*output)[ele],OPRFValueBytes);
                // auto temp = (uint8_t*)result[ele].data();
                // for(size_t idx = 0; idx < 16; idx ++){
                //     printf("%02x", temp[idx]);
                // }
                // printf("\n");
                
            }

            return std::move(output);
        }

        size_t OPRFReciver::getComCost(){
            return channel_.getBytesRecv()+channel_.getBytesSent();
        }



    } // namespace GCOPRF
    
} // namespace PSI
