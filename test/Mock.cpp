// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#include "mimic++/Mock.hpp"
#include "TestReporter.hpp"
#include "TestTypes.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

using namespace mimicpp;

TEMPLATE_TEST_CASE(
	"Mock is a non-copyable, but movable and default-constructible type.",
	"[mock]",
	void(),
	void() const,
	void() &,
	void() const &,
	void() &&,
	void() const &&,
	void() noexcept,
	void() const noexcept,
	void() & noexcept,
	void() const & noexcept,
	void() && noexcept,
	void() const && noexcept
)
{
	using MockT = Mock<TestType>;

	STATIC_REQUIRE(!std::is_copy_constructible_v<MockT>);
	STATIC_REQUIRE(!std::is_copy_assignable_v<MockT>);

	STATIC_REQUIRE(std::is_move_constructible_v<MockT>);
	STATIC_REQUIRE(std::is_move_assignable_v<MockT>);
	STATIC_REQUIRE(std::is_default_constructible_v<MockT>);

	STATIC_REQUIRE(std::is_nothrow_move_constructible_v<MockT>);
	STATIC_REQUIRE(std::is_nothrow_move_assignable_v<MockT>);
}

TEST_CASE(
	"Mutable Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void()> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int()> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Lvalue Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() &> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() &> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const lvalue Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const &> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const &> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Rvalue Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() &&> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call();
		CHECK(!expectation.is_satisfied());

		std::move(mock)();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() &&> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == std::move(mock)());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const rvalue Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const &&> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call();
		CHECK(!expectation.is_satisfied());

		std::move(mock)();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const &&> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == std::move(mock)());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Mutable noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Lvalue noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() & noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() & noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const lvalue noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const & noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call();
		CHECK(!expectation.is_satisfied());

		mock();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const & noexcept> mock{};

		const ScopedExpectation expectation = mock.expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == mock());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Rvalue noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() && noexcept> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call();
		CHECK(!expectation.is_satisfied());

		std::move(mock)();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() && noexcept> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == std::move(mock)());
		REQUIRE(expectation.is_satisfied());
	}
}

TEST_CASE(
	"Const rvalue noexcept Mock specialization supports expectations.",
	"[mock]"
)
{
	ScopedReporter reporter{};

	SECTION("With void() signature.")
	{
		Mock<void() const && noexcept> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call();
		CHECK(!expectation.is_satisfied());

		std::move(mock)();

		REQUIRE(expectation.is_satisfied());
	}

	SECTION("With int() signature.")
	{
		Mock<int() const && noexcept> mock{};

		const ScopedExpectation expectation = std::move(mock).expect_call()
											| finally::returns(42);
		CHECK(!expectation.is_satisfied());

		REQUIRE(42 == std::move(mock)());
		REQUIRE(expectation.is_satisfied());
	}
}
