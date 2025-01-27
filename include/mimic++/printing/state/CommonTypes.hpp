//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef MIMICPP_PRINTING_STATE_COMMON_TYPES_HPP
#define MIMICPP_PRINTING_STATE_COMMON_TYPES_HPP

#pragma once

#include "mimic++/Fwd.hpp"
#include "mimic++/String.hpp"
#include "mimic++/TypeTraits.hpp" // uint_with_size
#include "mimic++/printing/Format.hpp"
#include "mimic++/printing/Fwd.hpp"
#include "mimic++/printing/PathPrinter.hpp"
#include "mimic++/printing/state/Print.hpp"

#include <algorithm>
#include <bit>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <optional>
#include <source_location>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <>
struct mimicpp::format::formatter<mimicpp::ValueCategory, mimicpp::CharT>
    : public formatter<std::string_view, mimicpp::CharT>
{
    using ValueCategoryT = mimicpp::ValueCategory;

    auto format(
        const ValueCategoryT category,
        auto& ctx) const
    {
        constexpr auto toString = [](const ValueCategoryT cat) {
            switch (cat)
            {
            case ValueCategoryT::lvalue:
                return "lvalue";
            case ValueCategoryT::rvalue:
                return "rvalue";
            case ValueCategoryT::any:
                return "any";
            }

            throw std::invalid_argument{"Unknown category value."};
        };

        return formatter<std::string_view, mimicpp::CharT>::format(
            toString(category),
            ctx);
    }
};

template <>
struct mimicpp::format::formatter<mimicpp::Constness, mimicpp::CharT>
    : public formatter<std::string_view, mimicpp::CharT>
{
    using ConstnessT = mimicpp::Constness;

    auto format(
        const ConstnessT category,
        auto& ctx) const
    {
        constexpr auto toString = [](const ConstnessT value) {
            switch (value)
            {
            case ConstnessT::non_const:
                return "mutable";
            case ConstnessT::as_const:
                return "const";
            case ConstnessT::any:
                return "any";
            }

            throw std::invalid_argument{"Unknown constness value."};
        };

        return formatter<std::string_view, mimicpp::CharT>::format(
            toString(category),
            ctx);
    }
};

namespace mimicpp::printing::detail::state
{
    template <>
    struct common_type_printer<std::source_location>
    {
        template <print_iterator OutIter>
        static OutIter print(OutIter out, std::source_location const& loc)
        {
            out = print_path(std::move(out), loc.file_name());

            return format::format_to(
                std::move(out),
                "[{}:{}], {}",
                loc.line(),
                loc.column(),
                loc.function_name());
        }
    };

    template <>
    struct common_type_printer<std::nullopt_t>
    {
        template <print_iterator OutIter>
        static OutIter print(OutIter out, [[maybe_unused]] std::nullopt_t const)
        {
            return format::format_to(std::move(out), "nullopt");
        }
    };

    template <typename T>
    struct common_type_printer<std::optional<T>>
    {
        template <print_iterator OutIter>
        static OutIter print(OutIter out, auto& opt)
        {
            if (opt)
            {
                out = format::format_to(std::move(out), "{{ value: ");
                out = mimicpp::print(std::move(out), *opt);
                return format::format_to(std::move(out), " }}");
            }

            return mimicpp::print(std::move(out), std::nullopt);
        }
    };

    template <typename T>
    concept pointer_like = std::is_pointer_v<T>
                        || std::same_as<std::nullptr_t, T>;

    template <pointer_like T>
        requires(!string<T>)
    struct common_type_printer<T>
    {
        template <print_iterator OutIter>
        static OutIter print(OutIter out, T ptr)
        {
            if constexpr (4u < sizeof(T))
            {
                return format::format_to(
                    std::move(out),
                    "0x{:0>16x}",
                    std::bit_cast<std::uintptr_t>(ptr));
            }
            else
            {
                return format::format_to(
                    std::move(out),
                    "0x{:0>8x}",
                    std::bit_cast<std::uintptr_t>(ptr));
            }
        }
    };

    template <string String>
    struct common_type_printer<String>
    {
        template <std::common_reference_with<String> T, print_iterator OutIter>
        static OutIter print(OutIter out, T&& str)
        {
            if constexpr (constexpr auto prefix = string_literal_prefix<string_char_t<String>>;
                          !std::ranges::empty(prefix))
            {
                out = std::ranges::copy(prefix, std::move(out)).out;
            }

            out = format::format_to(std::move(out), "\"");

            if constexpr (std::same_as<CharT, string_char_t<String>>)
            {
                out = std::ranges::copy(
                          string_traits<String>::view(str),
                          std::move(out))
                          .out;
            }
            // required for custom char types
            else if constexpr (printer_for<custom::Printer<string_char_t<String>>, OutIter, string_char_t<String>>)
            {
                for (custom::Printer<string_char_t<String>> printer{};
                     const string_char_t<String>& c : string_traits<String>::view(str))
                {
                    out = printer.print(std::move(out), c);
                }
            }
            else
            {
                constexpr auto to_dump = [](const string_char_t<String>& c) noexcept {
                    using intermediate_t = uint_with_size_t<sizeof c>;
                    return std::bit_cast<intermediate_t>(c);
                };

                auto view = string_traits<std::remove_cvref_t<T>>::view(std::forward<T>(str));
                auto iter = std::ranges::begin(view);
                if (const auto end = std::ranges::end(view);
                    iter != end)
                {
                    out = format::format_to(
                        std::move(out),
                        "{:#x}",
                        to_dump(*iter++));

                    for (; iter != end; ++iter)
                    {
                        out = format::format_to(
                            std::move(out),
                            ", {:#x}",
                            to_dump(*iter));
                    }
                }
            }

            return format::format_to(std::move(out), "\"");
        }
    };
}

#endif