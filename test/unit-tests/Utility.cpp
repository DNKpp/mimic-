//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "mimic++/Utility.hpp"
#include "mimic++/Printer.hpp"

#include <cstdint>
#include <tuple>

using namespace mimicpp;

TEMPLATE_TEST_CASE(
    "to_underlying converts the enum value to its underlying representation.",
    "[utility]",
    std::int8_t,
    std::uint8_t,
    std::int16_t,
    std::uint16_t,
    std::int32_t,
    std::uint32_t,
    std::int64_t,
    std::uint64_t)
{
    using UnderlyingT = TestType;

    SECTION("When an enum value is given.")
    {
        enum Test : UnderlyingT
        {
        };

        const UnderlyingT value = GENERATE(
            std::numeric_limits<UnderlyingT>::min(),
            0,
            1,
            std::numeric_limits<UnderlyingT>::max());

        STATIC_REQUIRE(std::same_as<UnderlyingT, decltype(to_underlying(Test{value}))>);
        REQUIRE(value == to_underlying(Test{value}));
    }

    SECTION("When an class enum value is given.")
    {
        enum class Test : UnderlyingT
        {
        };

        const UnderlyingT value = GENERATE(
            std::numeric_limits<UnderlyingT>::min(),
            0,
            1,
            std::numeric_limits<UnderlyingT>::max());

        STATIC_REQUIRE(std::same_as<UnderlyingT, decltype(to_underlying(Test{value}))>);
        REQUIRE(value == to_underlying(Test{value}));
    }
}

TEMPLATE_TEST_CASE_SIG(
    "satisfies determines, whether T satisfies the given trait.",
    "[utility]",
    ((bool expected, typename T, template <typename> typename Trait), expected, T, Trait),
    (false, int, std::is_floating_point),
    (false, int&, std::is_integral),
    (true, int, std::is_integral))
{
    STATIC_REQUIRE(expected == satisfies<T, Trait>);
}

TEMPLATE_TEST_CASE_SIG(
    "same_as_any determines, whether T is the same as any other given type.",
    "[utility]",
    ((bool expected, typename T, typename... Others), expected, T, Others...),
    (false, int),
    (false, int, int&),
    (false, int, int&, int&),
    (false, int, int&, double, float),
    (true, int, int),
    (true, int, int&, int),
    (true, int, double, int, int&))
{
    STATIC_REQUIRE(expected == same_as_any<T, Others...>);
}

TEMPLATE_TEST_CASE_SIG(
    "unique_list_t is an alias to a tuple with just unique types.",
    "[utility]",
    ((bool dummy, typename Expected, typename... Types), dummy, Expected, Types...),
    (true, type_list<>),
    (true, type_list<int, int&>, int, int&),
    (true, type_list<int, int&>, int, int&, int&),
    (true, type_list<int, int&, double, float>, int, int&, double, float),
    (true, type_list<int>, int, int),
    (true, type_list<int, int&>, int, int&, int),
    (true, type_list<int, double, int&>, int, double, int, int&))
{
    STATIC_REQUIRE(std::same_as<Expected, detail::unique_list_t<Types...>>);
}

TEST_CASE(
    "detail::expand_tuple appends elements, until the desired size is reached.",
    "[detail][utility]")
{
    // on clang-19, libc++ and c++23, the direct tuple comparisons won't compile.
    // so use std::equal_to as workaround.
    // see: https://github.com/llvm/llvm-project/issues/113087

    SECTION("When tuple is empty")
    {
        std::tuple input{};

        SECTION("And n == 0")
        {
            const std::tuple result = detail::expand_tuple<int, 0>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<>, decltype(result)>);
        }

        SECTION("And n == 1")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{0}, result));
        }

        SECTION("And n == 2")
        {
            const std::tuple result = detail::expand_tuple<int, 2>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<int, int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{0, 0}, result));
        }
    }

    SECTION("When tuple has size 1")
    {
        std::tuple input{42.};

        SECTION("And n == 1")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double>, decltype(result)>);
        }

        SECTION("And n == 2")
        {
            const std::tuple result = detail::expand_tuple<int, 2>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double, int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{42., 0}, result));
        }

        SECTION("And n == 3")
        {
            const std::tuple result = detail::expand_tuple<int, 3>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double, int, int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{42., 0, 0}, result));
        }
    }

    SECTION("When tuple has size 2")
    {
        std::tuple input{1337., 42.f};

        SECTION("And n == 2")
        {
            const std::tuple result = detail::expand_tuple<int, 2>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double, float>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{1337., 42.f}, result));
        }

        SECTION("And n == 3")
        {
            const std::tuple result = detail::expand_tuple<int, 3>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double, float, int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{1337., 42.f, 0}, result));
        }

        SECTION("And n == 4")
        {
            const std::tuple result = detail::expand_tuple<int, 4>(std::move(input));
            STATIC_REQUIRE(std::same_as<const std::tuple<double, float, int, int>, decltype(result)>);
            REQUIRE(std::equal_to{}(std::tuple{1337., 42.f, 0, 0}, result));
        }
    }

    SECTION("It keeps the existing elemens as they are.")
    {
        std::string arg{"Hello, World"};

        SECTION("When argument is const.")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::tuple<const std::string>(arg));
            STATIC_REQUIRE(std::same_as<const std::tuple<const std::string>, decltype(result)>);
            REQUIRE(arg == std::get<0>(result));
        }

        SECTION("When argument is lvalue-ref.")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::tuple<std::string&>(arg));
            STATIC_REQUIRE(std::same_as<const std::tuple<std::string&>, decltype(result)>);
            REQUIRE(&arg == &std::get<0>(result));
        }

        SECTION("When argument is const lvalue-ref.")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::tuple<const std::string&>(arg));
            STATIC_REQUIRE(std::same_as<const std::tuple<const std::string&>, decltype(result)>);
            REQUIRE(&arg == &std::get<0>(result));
        }

        SECTION("When argument is rvalue-ref.")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::tuple<std::string&&>(std::move(arg)));
            STATIC_REQUIRE(std::same_as<const std::tuple<std::string&&>, decltype(result)>);
            REQUIRE(&arg == &std::get<0>(result));
        }

        SECTION("When argument is const rvalue-ref.")
        {
            const std::tuple result = detail::expand_tuple<int, 1>(std::tuple<const std::string&&>(std::move(arg)));
            STATIC_REQUIRE(std::same_as<const std::tuple<const std::string&&>, decltype(result)>);
            REQUIRE(&arg == &std::get<0>(result));
        }
    }
}
