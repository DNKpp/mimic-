// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#include "mimic++/Utility.hpp"
#include "mimic++/Printer.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cstdint>

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
	std::uint64_t
)
{
	using UnderlyingT = TestType;

	SECTION("When an enum value is given.")
	{
		enum Test
			: UnderlyingT
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
		enum class Test
			: UnderlyingT
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

TEST_CASE(
	"ValueCategory is formattable.",
	"[utility]"
)
{
	namespace Matches = Catch::Matchers;

	SECTION("When valid ValueCategory is given.")
	{
		const auto [expected, category] = GENERATE(
			(table<StringT, ValueCategory>)({
				{"any", ValueCategory::any},
				{"rvalue",ValueCategory::rvalue},
				{"lvalue", ValueCategory::lvalue},
				}));

		REQUIRE_THAT(
			format::format("{}", category),
			Matches::Equals(expected));
	}

	SECTION("When an invalid ValueCategory is given, std::invalid_argument is thrown.")
	{
		REQUIRE_THROWS_AS(
			format::format("{}", ValueCategory{42}),
			std::invalid_argument);
	}
}

TEST_CASE(
	"Constness is formattable.",
	"[utility]"
)
{
	namespace Matches = Catch::Matchers;

	SECTION("When valid Constness is given.")
	{
		const auto [expected, category] = GENERATE(
			(table<StringT, Constness>)({
				{"any", Constness::any},
				{"const",Constness::as_const},
				{"mutable", Constness::non_const},
				}));

		REQUIRE_THAT(
			format::format("{}", category),
			Matches::Equals(expected));
	}

	SECTION("When an invalid Constness is given, std::invalid_argument is thrown.")
	{
		REQUIRE_THROWS_AS(
			format::format("{}", Constness{42}),
			std::invalid_argument);
	}
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
	(true, int, double, int, int&)
)
{
	STATIC_REQUIRE(expected == same_as_any<T, Others...>);
}

TEMPLATE_TEST_CASE_SIG(
	"unique_list_t is an alias to a tuple with just unique types.",
	"[utility]",
	((bool dummy, typename Expected, typename... Types), dummy, Expected, Types...),
	(true, std::tuple<>),
	(true, std::tuple<int, int&>, int, int&),
	(true, std::tuple<int, int&>, int, int&, int&),
	(true, std::tuple<int, int&, double, float>, int, int&, double, float),
	(true, std::tuple<int>, int, int),
	(true, std::tuple<int, int&>, int, int&, int),
	(true, std::tuple<int, double, int&>, int, double, int, int&)
)
{
	STATIC_REQUIRE(std::same_as<Expected, detail::unique_list_t<Types...>>);
}
