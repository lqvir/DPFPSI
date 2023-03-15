#pragma once

// STD
#include <cstddef>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <set>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>
#include <span>
#include "gsl/span"
#include "openssl/evp.h"

namespace PSI{
    namespace util{
              /**
        Writes a vector into an std::ostream as [a, b, c, ..., z].
        */
        template <typename T, std::size_t Extent, typename ToString>
        std::string to_string(std::span<T, Extent> values, ToString to_string_fun)
        {
            if (values.empty()) {
                return "[ ]";
            }

            std::stringstream ss;
            ss << "[";
            for (std::size_t i = 0; i < values.size() - 1; i++) {
                ss << to_string_fun(values[i]) << ", ";
            }
            ss << to_string_fun(values.back()) << "]";

            return ss.str();
        }

        /**
        Writes a vector into an std::ostream as [a, b, c, ..., z].
        */
        template <typename T, std::size_t Extent>
        std::string to_string(std::span<T, Extent> values)
        {
            return to_string(values, [](T &t) -> T & { return t; });
        }

        /**
        Writes a vector into an std::ostream as [a, b, c, ..., z].
        */
        template <typename T, typename ToString>
        std::string to_string(const std::vector<T> &values, ToString to_string_fun)
        {
            return to_string(gsl::span<const T>(values), to_string_fun);
        }

        /**
        Writes a vector into an std::ostream as [a, b, c, ..., z].
        */
        template <typename T>
        std::string to_string(const std::vector<T> &values)
        {
            return to_string(std::span<const T>(values));
        }

        /**
        Writes a set into an std::ostream as {a, b, c, ..., z}.
        */
        template <typename T, typename ToString>
        std::string to_string(const std::set<T> &values, ToString to_string_fun)
        {
            if (values.empty()) {
                return "{ }";
            }

            std::stringstream ss;
            ss << "{";
            using values_diff_type = typename std::decay_t<decltype(values)>::difference_type;
            auto values_last =
                std::next(values.cbegin(), static_cast<values_diff_type>(values.size() - 1));
            for (auto it = values.cbegin(); it != values_last; it++) {
                ss << to_string_fun(*it) << ", ";
            }
            ss << to_string_fun(*values_last) << "}";

            return ss.str();
        }

        /**
        Writes a set into an std::ostream as {a, b, c, ..., z}.
        */
        template <typename T>
        std::string to_string(const std::set<T> &values)
        {
            return to_string(values, [](const T &t) -> const T & { return t; });
        }

        
        /**
        Computes the XOR of two byte buffers.
        */
        void xor_buffers(unsigned char *buf1, const unsigned char *buf2, std::size_t count);

        /**
        Copies bytes from source to destination, throwing an exception if either source or
        destination is nullptr. The memory ranges are assumed to be non-overlapping.
        */
        void copy_bytes(const void *src, std::size_t count, void *dst);

        /**
        Returns whether two byte buffers of given length hold the same data, throwing an exception
        if either pointer is nullptr.
        */
        bool compare_bytes(const void *first, const void *second, std::size_t count);

        void printchar(char* in,size_t len);
        void printchar(unsigned char* in,size_t len);
        int blake2b512(void* in,size_t inlen,void* out,size_t out_len);

    } //namespace util

}  // namespace PSI
