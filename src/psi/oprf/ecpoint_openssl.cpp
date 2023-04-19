#include "ecpoint_openssl.h"

// STD
#include <algorithm>
#include <functional>

// FourQ
#include "psi/fourq/FourQ_api.h"
#include "psi/fourq/FourQ_internal.h"
#include "psi/fourq/random.h"

#include "psi/common/utils.h"
namespace PSI
{
    namespace OPRF
    {
        namespace {
           void random_scalar(ECPointFourQ::scalar_span_type value){
                random_bytes(value.data(), value.size());
                modulo_order(
                    reinterpret_cast<digit_t *>(value.data()),
                    reinterpret_cast<digit_t *>(value.data()));
            }

            digit_t is_nonzero_scalar(ECPointFourQ::scalar_span_type value)
            {
                const digit_t *value_ptr = reinterpret_cast<digit_t *>(value.data());
                digit_t c = 0;

                for (size_t i = 0; i < NWORDS_ORDER; i++) {
                    c |= value_ptr[i];
                }

                sdigit_t first_nz = -static_cast<sdigit_t>(c & 1);
                sdigit_t rest_nz = -static_cast<sdigit_t>(c >> 1);
                return static_cast<digit_t>((first_nz | rest_nz) >> (8 * sizeof(digit_t) - 1));
            }
        }
        ECPointFourQ::ECPointFourQ(input_span_const_type value)
        {
            // util::printchar((uint8_t*)value.data(),value.size() );
            if (!value.empty()) {
                f2elm_t r;

                // // Compute a Blake2b hash of the value
                // APSI_blake2b(
                //     reinterpret_cast<unsigned char *>(r),
                //     sizeof(f2elm_t),
                //     value.data(),
                //     static_cast<size_t>(value.size()),
                //     nullptr,
                //     0);
                
                util::blake2b512(
                    (void*)value.data(),
                    value.size(),
                    (void*)r,
                    sizeof(r)
                );
                //  util::printchar((uint8_t*)r,sizeof(f2elm_t) );

                // Reduce r; note that this does not produce a perfectly uniform distribution modulo
                // 2^127-1, but it is good enough.
                mod1271(r[0]);
                mod1271(r[1]);

                // Create an elliptic curve point
                HashToCurve(r, pt_);

                // point_t pt_copy{ pt_[0] };
                // point_extproj_t R;
                // point_setup(pt_copy, R);
                // if (ecc_point_validate(R) == false) {
                //     fpneg1271(R->x[1]);
                //     fpcopy1271(R->x[1], pt_copy->x[1]);
                //     if (ecc_point_validate(R) == false) { // Final point validation
                //         std::cout << "error" << std::endl;
                //     }
                // }
            }
        }

        void ECPointFourQ::MakeRandomNonzeroScalar(scalar_span_type out)
        {
            // Loop until we find a non-zero element
            do {
                random_scalar(out);
            } while (!is_nonzero_scalar(out));
        }

        void ECPointFourQ::InvertScalar(scalar_span_const_type in, scalar_span_type out)
        {
            to_Montgomery(
                const_cast<digit_t *>(reinterpret_cast<const digit_t *>(in.data())),
                reinterpret_cast<digit_t *>(out.data()));
            Montgomery_inversion_mod_order(
                reinterpret_cast<digit_t *>(out.data()), reinterpret_cast<digit_t *>(out.data()));
            from_Montgomery(
                reinterpret_cast<digit_t *>(out.data()), reinterpret_cast<digit_t *>(out.data()));
        }

        bool ECPointFourQ::scalar_multiply(scalar_span_const_type scalar, bool clear_cofactor)
        {
            // The ecc_mul functions returns false when the input point is not a valid curve point
            return ecc_mul(
                pt_,
                const_cast<digit_t *>(reinterpret_cast<const digit_t *>(scalar.data())),
                pt_,
                clear_cofactor);
        }

        ECPointFourQ &ECPointFourQ::operator=(const ECPointFourQ &assign)
        {
            if (&assign != this) {
                pt_[0] = assign.pt_[0];
            }
            return *this;
        }
    void ECPointFourQ::save(std::ostream &stream) const
        {
            auto old_ex_mask = stream.exceptions();
            stream.exceptions(std::ios_base::failbit | std::ios_base::badbit);
            
            try {
                std::array<unsigned char, save_size> buf;
                point_t pt_copy{ pt_[0] };
                encode(pt_copy, buf.data());
                stream.write(reinterpret_cast<const char *>(buf.data()), save_size);
            } catch (const std::ios_base::failure &) {
                stream.exceptions(old_ex_mask);
                throw;
            }
            stream.exceptions(old_ex_mask);
        }

        void ECPointFourQ::load(std::istream &stream)
        {
            auto old_ex_mask = stream.exceptions();
            stream.exceptions(std::ios_base::failbit | std::ios_base::badbit);

            try {
                std::array<unsigned char, save_size> buf;
                stream.read(reinterpret_cast<char *>(buf.data()), save_size);
                if (decode(buf.data(), pt_) != ECCRYPTO_SUCCESS) {
                    stream.exceptions(old_ex_mask);
                    throw std::logic_error("invalid point");
                }
            } catch (const std::ios_base::failure &) {
                stream.exceptions(old_ex_mask);
                throw;
            }
            stream.exceptions(old_ex_mask);
        }

        void ECPointFourQ::save(point_save_span_type out) const
        {
            point_t pt_copy{ pt_[0] };
            encode(pt_copy, out.data());
            // util::printchar(out.data(),out.size());
         

        }

        void ECPointFourQ::load(point_save_span_const_type in)
        {
            // util::printchar((uint8_t*)in.data(),in.size());

            if (decode(in.data(), pt_) != ECCRYPTO_SUCCESS) {
                // std::cout << __LINE__ <<std::endl;
                throw std::logic_error("invalid point");
            }
        }

        void ECPointFourQ::extract_hash(hash_span_type out) const
        {
            // Compute a Blake2b hash of the value and expand to hash_size
            util::blake2b512((void*)pt_->y,sizeof(f2elm_t),out.data(),out.size());
        }


    } // namespace OPRF
    
} // namespace PSI
