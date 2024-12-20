// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#ifndef MIMICPP_STACKTRACE_HPP
#define MIMICPP_STACKTRACE_HPP

#pragma once

#include "Stacktrace.hpp"
#include "mimic++/Utility.hpp"

#include <any>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>

namespace mimicpp::custom
{
    template <typename Tag>
    struct find_stacktrace_backend;
}

namespace mimicpp
{
    struct register_tag
    {
    };

    template <typename Tag>
    struct find_stacktrace_backend;

    template <typename Backend>
    struct stacktrace_traits;

    class Stacktrace
    {
    public:
        using size_fn = std::size_t (*)(const std::any&);
        using empty_fn = bool (*)(const std::any&);
        using description_fn = std::string (*)(const std::any&, std::size_t);
        using source_file_fn = std::string (*)(const std::any&, std::size_t);
        using source_line_fn = std::size_t (*)(const std::any&, std::size_t);

        ~Stacktrace() = default;

        template <typename Inner, typename Traits = stacktrace_traits<std::remove_cvref_t<Inner>>>
            requires(!std::same_as<Stacktrace, std::remove_cvref_t<Inner>>)
        [[nodiscard]]
        explicit constexpr Stacktrace(Inner&& inner)
            : m_Inner{std::forward<Inner>(inner)},
              m_SizeFn{&Traits::size},
              m_EmptyFn{&Traits::empty},
              m_DescriptionFn{&Traits::description},
              m_SourceFileFn{&Traits::source_file},
              m_SourceLineFn{&Traits::source_line}
        {
        }

        [[nodiscard]]
        Stacktrace(Stacktrace&&) = default;
        Stacktrace& operator=(Stacktrace&&) = default;

        Stacktrace(const Stacktrace&) = delete;
        Stacktrace& operator=(const Stacktrace&) = delete;

        [[nodiscard]]
        std::size_t size() const
        {
            return std::invoke(m_SizeFn, m_Inner);
        }

        [[nodiscard]]
        bool empty() const
        {
            return std::invoke(m_EmptyFn, m_EmptyFn);
        }

        [[nodiscard]]
        std::string description(const std::size_t at) const
        {
            return std::invoke(m_DescriptionFn, m_Inner, at);
        }

        [[nodiscard]]
        std::string source_file(const std::size_t at) const
        {
            return std::invoke(m_SourceFileFn, m_Inner, at);
        }

        [[nodiscard]]
        std::size_t source_line(const std::size_t at) const
        {
            return std::invoke(m_SourceLineFn, m_Inner, at);
        }

    private:
        std::any m_Inner;
        size_fn m_SizeFn;
        empty_fn m_EmptyFn;
        description_fn m_DescriptionFn;
        source_file_fn m_SourceFileFn;
        source_line_fn m_SourceLineFn;
    };

    namespace detail::stacktrace_current_hook
    {
        template <typename Tag, typename FindBackendT = custom::find_stacktrace_backend<Tag>>
            requires requires {
                {
                    stacktrace_traits<
                        typename FindBackendT::type>::current(std::size_t{})
                } -> std::convertible_to<typename FindBackendT::type>;
                requires std::constructible_from<Stacktrace, typename FindBackendT::type>;
            }
        [[nodiscard]]
        constexpr auto current([[maybe_unused]] const priority_tag<2>, const std::size_t skip)
        {
            return stacktrace_traits<
                typename FindBackendT::type>::current(skip + 1);
        }

        template <typename Tag, typename FindBackendT = find_stacktrace_backend<Tag>>
            requires requires {
                {
                    stacktrace_traits<
                        typename FindBackendT::type>::current(std::size_t{})
                } -> std::convertible_to<typename FindBackendT::type>;
                requires std::constructible_from<Stacktrace, typename FindBackendT::type>;
            }
        [[nodiscard]]
        constexpr auto current([[maybe_unused]] const priority_tag<1>, const std::size_t skip)
        {
            return stacktrace_traits<
                typename FindBackendT::type>::current(skip + 1);
        }

        template <typename Tag>
        constexpr auto current([[maybe_unused]] const priority_tag<0>, [[maybe_unused]] const std::size_t skip)
        {
            static_assert(
                always_false<Tag>{},
                "mimic++ does not have a registered stacktrace-backend.");
        }

        constexpr priority_tag<2> maxTag;

        struct current_fn
        {
            template <typename Tag = register_tag>
            [[nodiscard]]
            constexpr Stacktrace operator()(const std::size_t skip) const
            {
                return Stacktrace{
                    stacktrace_current_hook::current<Tag>(maxTag, skip + 1)};
            }

            template <typename Tag = register_tag>
            [[nodiscard]]
            constexpr Stacktrace operator()() const
            {
                return Stacktrace{
                    stacktrace_current_hook::current<Tag>(maxTag, 1u)};
            }
        };
    }

    [[maybe_unused]]
    constexpr detail::stacktrace_current_hook::current_fn current_stacktrace{};
}

#ifdef MIMICPP_CONFIG_EXPERIMENTAL_STACKTRACE

    #ifdef MIMICPP_CONFIG_USE_CPPTRACE

        #if __has_include(<cpptrace/basic.hpp>)
            #include <cpptrace/basic.hpp>
        #else
            #error "The cpptrace stacktrace backend is explicitly enabled, but the include <cpptrace/basic.hpp> can not be found."
        #endif

namespace mimicpp::cpptrace
{
    using namespace ::cpptrace;

    class Backend
    {
    public:
        ~Backend() = default;

        [[nodiscard]]
        explicit Backend(raw_trace&& trace) noexcept
            : m_Trace{std::move(trace)}
        {
        }

        Backend(const Backend&) = default;
        Backend& operator=(const Backend&) = default;
        Backend(Backend&&) = default;
        Backend& operator=(Backend&&) = default;

        [[nodiscard]]
        const stacktrace& data() const
        {
            if (const auto* raw = std::get_if<raw_trace>(&m_Trace))
            {
                m_Trace = raw->resolve();
            }

            return std::get<stacktrace>(m_Trace);
        }

    private:
        using TraceT = std::variant<raw_trace, stacktrace>;
        mutable TraceT m_Trace;
    };

    // std::any requires this
    // see: https://en.cppreference.com/w/cpp/named_req/CopyConstructible
    static_assert(std::is_copy_constructible_v<Backend>);
}

template <>
struct mimicpp::find_stacktrace_backend<mimicpp::register_tag>
{
    using type = cpptrace::Backend;
};

template <>
struct mimicpp::stacktrace_traits<mimicpp::cpptrace::Backend>
{
    using type = cpptrace::Backend;

    [[nodiscard]]
    static type current(const std::size_t skip)
    {
        return type{::cpptrace::generate_raw_trace(skip + 1)};
    }

    [[nodiscard]]
    static std::size_t size(const std::any& storage)
    {
        return get(storage).frames.size();
    }

    [[nodiscard]]
    static bool empty(const std::any& storage)
    {
        return get(storage).empty();
    }

    [[nodiscard]]
    static std::string description(const std::any& storage, const std::size_t at)
    {
        return get_frame(storage, at).symbol;
    }

    [[nodiscard]]
    static std::string source_file(const std::any& storage, const std::size_t at)
    {
        return get_frame(storage, at).filename;
    }

    [[nodiscard]]
    static std::size_t source_line(const std::any& storage, const std::size_t at)
    {
        return get_frame(storage, at).line.value_or(0u);
    }

    [[nodiscard]]
    static const cpptrace::stacktrace& get(const std::any& storage)
    {
        return std::any_cast<const BackendT&>(storage).data();
    }

    [[nodiscard]]
    static const cpptrace::stacktrace_frame& get_frame(const std::any& storage, const std::size_t at)
    {
        return get(storage).frames.at(at);
    }
};

    #endif

#endif

#endif