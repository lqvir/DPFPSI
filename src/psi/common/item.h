#pragma once

#include <iostream>
#include <immintrin.h>
#include <span>
#include "psi/common/utils.h"
#include "psi/param.h"
#include "gsl/span"
namespace PSI{



template<typename value_type>
class ItemTmpl {
    public:
        

        /**
        Constructs a zero item.
        */
        ItemTmpl() = default;

        ItemTmpl(const value_type &value) : value_(value)
        {}

        ItemTmpl(const ItemTmpl &) = default;

        ItemTmpl(ItemTmpl &&) = default;

        ItemTmpl &operator=(const ItemTmpl &item) = default;

        ItemTmpl &operator=(ItemTmpl &&item) = default;

        /**
        Constructs an ItemTmpl by hashing a given string of arbitrary length.
        */
        template <typename CharT>
        ItemTmpl(const std::basic_string<CharT> &str)
        {
            operator=<CharT>(str);
        }
        template <typename CharT>
        ItemTmpl(const gsl::span<CharT> &str)
        {
            if (str.empty()) {
                throw std::invalid_argument("str cannot be empty");
            }
            util::copy_bytes(str.data(),str.size()*sizeof(CharT),value_.data());
        }
        template <typename CharT>
        ItemTmpl(const std::span<CharT> &str)
        {
            if (str.empty()) {
                throw std::invalid_argument("str cannot be empty");
            }
            util::copy_bytes(str.data(),str.size()*sizeof(CharT),value_.data());
        }
        /**
        Hash a given string of arbitrary length into an ItemTmpl.
        */
        template <typename CharT>
        ItemTmpl &operator=(const std::basic_string<CharT> &str)
        {
            if (str.empty()) {
                throw std::invalid_argument("str cannot be empty");
            }
            util::copy_bytes(str.data(),str.size()*sizeof(CharT),value_.data());
            // copy_bytes(str.data(), str.size() * sizeof(CharT));
            return *this;
        }

        // /**
        // Returns the Bitstring representing this ItemTmpl's data.
        // */
        // Bitstring to_bitstring(std::uint32_t item_bit_count) const;

        bool operator==(const ItemTmpl &other) const
        {
            return value_ == other.value_;
        }

        /**
        Returns a span of a desired (standard layout) type to the label data.
        */
        template <typename T, typename = std::enable_if_t<std::is_standard_layout<T>::value>>
        auto get_as() const
        {
            constexpr std::size_t count = sizeof(value_) / sizeof(T);
            return gsl::span<std::add_const_t<T>, count>(
                reinterpret_cast<std::add_const_t<T> *>(value_.data()), count);
        }

        /**
        Returns a span of a desired (standard layout) type to the label data.
        */
        template <typename T, typename = std::enable_if_t<std::is_standard_layout<T>::value>>
        auto get_as()
        {
            constexpr std::size_t count = sizeof(value_) / sizeof(T);
            return gsl::span<T, count>(reinterpret_cast<T *>(value_.data()), count);
        }

        // only for std::array<uint8_t,2>
        ItemTmpl(std::uint64_t lw, std::uint64_t hw)
        {
            auto words = get_as<uint64_t>();
            words[0] = lw;
            words[1] = hw;
        }

        value_type value() const
        {
            return value_;
        }

        value_type &value()
        {
            return value_;
        }
        std::string to_string() const
        {
            return util::to_string(get_as<uint32_t>());
        }

        bool test(size_t idx){
            size_t byte_idx = idx >> 3;
            size_t bit_idx = idx & 7;
            return (bool)(value_[byte_idx]&bit_mask[bit_idx]);
        }

    private:
        // void hash_to_value(const void *in, std::size_t size);

        value_type value_{};
    }; // class Item

    using Item = ItemTmpl<std::array<uint8_t,Item_byte_size>>;
    using Label = ItemTmpl<std::array<uint8_t,Label_byte_size>>;
    using LabelMask = ItemTmpl<std::array<uint8_t,Label_byte_size+Leading_zero_length>>;

    
    

    LabelMask xor_LabelMask(const LabelMask& buf1,const LabelMask& buf2);
    Label toLabel(const LabelMask& buf);
} // namespace PSI