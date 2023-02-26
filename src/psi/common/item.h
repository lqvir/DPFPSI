#pragma once

#include <iostream>
#include <immintrin.h>
#include "gsl/span"
#include "psi/common/utils.h"
namespace PSI{
class Item {
    public:
        using value_type = std::array<unsigned char, 16>;

        /**
        Constructs a zero item.
        */
        Item() = default;

        Item(const value_type &value) : value_(value)
        {}

        // /**
        // Constructs an Item from a BitstringView. This throws std::invalid_argument if the bitstring
        // doesn't fit into std::array<unsigned char, 16>.
        // */
        // template <typename T>
        // Item(const BitstringView<T> &bitstring)
        // {
        //     auto bitstring_bytes = bitstring.data();
        //     if (bitstring_bytes.size() > sizeof(value_)) {
        //         throw std::invalid_argument("bitstring is too long to fit into an Item");
        //     }

        //     std::copy(bitstring_bytes.begin(), bitstring_bytes.end(), value_.begin());
        // }

        Item(const Item &) = default;

        Item(Item &&) = default;

        Item &operator=(const Item &item) = default;

        Item &operator=(Item &&item) = default;

        /**
        Constructs an Item by hashing a given string of arbitrary length.
        */
        template <typename CharT>
        Item(const std::basic_string<CharT> &str)
        {
            operator=<CharT>(str);
        }

        /**
        Hash a given string of arbitrary length into an Item.
        */
        template <typename CharT>
        Item &operator=(const std::basic_string<CharT> &str)
        {
            if (str.empty()) {
                throw std::invalid_argument("str cannot be empty");
            }
            util::copy_bytes(str.data(),str.size()*sizeof(CharT),value_.data());
            // copy_bytes(str.data(), str.size() * sizeof(CharT));
            return *this;
        }

        // /**
        // Returns the Bitstring representing this Item's data.
        // */
        // Bitstring to_bitstring(std::uint32_t item_bit_count) const;

        bool operator==(const Item &other) const
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

        Item(std::uint64_t lw, std::uint64_t hw)
        {
            auto words = get_as<std::uint64_t>();
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

        std::string to_string() const;

    private:
        // void hash_to_value(const void *in, std::size_t size);

        value_type value_{};
    }; // class Item

}