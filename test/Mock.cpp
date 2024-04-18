// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#include "mimic++/Mock.hpp"
#include "mimic++/ExpectationPolicies/CallProperties.hpp"

#include "TestReporter.hpp"
#include "TestTypes.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>

TEMPLATE_TEST_CASE(
	"Mocks are non-copyable but movable and can be default constructed.",
	"[mock]",
	mimicpp::Mock<void()>,
	mimicpp::Mock<void(int)>,
	mimicpp::Mock<void(int, double)>,
	mimicpp::Mock<int()>,
	mimicpp::Mock<int(float)>,
	mimicpp::Mock<int(float, double)>
)
{
	STATIC_REQUIRE(!std::is_copy_constructible_v<TestType>);
	STATIC_REQUIRE(!std::is_copy_assignable_v<TestType>);

	STATIC_REQUIRE(std::is_move_constructible_v<TestType>);
	STATIC_REQUIRE(std::is_move_assignable_v<TestType>);
	STATIC_REQUIRE(std::is_default_constructible_v<TestType>);
}

TEST_CASE("Mock<void()>::expect_call expectes a call with the same category and constness.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	SECTION("Expect const lvalue call.")
	{
		SECTION("As const lvalue succeeds.")
		{
			{
				ExpectationT expectation = std::as_const(mock).expect_call();
				REQUIRE_NOTHROW(std::as_const(mock)());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		SECTION("Any other fails.")
		{
			{
				ExpectationT expectation = std::as_const(mock).expect_call();

				SECTION("As lvalue.")
				{
					REQUIRE_THROWS_AS(
						mock(),
						TestExpectationError);
				}

				SECTION("As const rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(std::as_const(mock))(),
						TestExpectationError);
				}

				SECTION("As rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(mock)(),
						TestExpectationError);
				}

				REQUIRE_THAT(
					reporter.no_match_reports(),
					Catch::Matchers::SizeIs(1));
				REQUIRE_THAT(
					reporter.exhausted_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.ok_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.unsatisfied_expectations(),
					Catch::Matchers::IsEmpty());
			}

			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::SizeIs(1));
		}
	}

	SECTION("Expect lvalue call.")
	{
		SECTION("As lvalue succeeds.")
		{
			{
				ExpectationT expectation = mock.expect_call();
				REQUIRE_NOTHROW(mock());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		SECTION("Any other fails.")
		{
			{
				ExpectationT expectation = mock.expect_call();

				SECTION("As const lvalue.")
				{
					REQUIRE_THROWS_AS(
						std::as_const(mock)(),
						TestExpectationError);
				}

				SECTION("As const rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(std::as_const(mock))(),
						TestExpectationError);
				}

				SECTION("As rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(mock)(),
						TestExpectationError);
				}

				REQUIRE_THAT(
					reporter.no_match_reports(),
					Catch::Matchers::SizeIs(1));
				REQUIRE_THAT(
					reporter.exhausted_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.ok_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.unsatisfied_expectations(),
					Catch::Matchers::IsEmpty());
			}

			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::SizeIs(1));
		}
	}

	SECTION("Expect const rvalue call.")
	{
		SECTION("As const rvalue succeeds.")
		{
			{
				ExpectationT expectation = std::move(std::as_const(mock)).expect_call();
				REQUIRE_NOTHROW(std::move(std::as_const(mock))());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		SECTION("Any other fails.")
		{
			{
				ExpectationT expectation = std::move(std::as_const(mock)).expect_call();

				SECTION("As const lvalue.")
				{
					REQUIRE_THROWS_AS(
						std::as_const(mock)(),
						TestExpectationError);
				}

				SECTION("As lvalue.")
				{
					REQUIRE_THROWS_AS(
						mock(),
						TestExpectationError);
				}

				SECTION("As rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(mock)(),
						TestExpectationError);
				}

				REQUIRE_THAT(
					reporter.no_match_reports(),
					Catch::Matchers::SizeIs(1));
				REQUIRE_THAT(
					reporter.exhausted_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.ok_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.unsatisfied_expectations(),
					Catch::Matchers::IsEmpty());
			}

			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::SizeIs(1));
		}
	}

	SECTION("Expect rvalue call.")
	{
		SECTION("As rvalue succeeds.")
		{
			{
				ExpectationT expectation = std::move(mock).expect_call();
				REQUIRE_NOTHROW(std::move(mock)());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		SECTION("Any other fails.")
		{
			{
				ExpectationT expectation = std::move(mock).expect_call();

				SECTION("As const lvalue.")
				{
					REQUIRE_THROWS_AS(
						std::as_const(mock)(),
						TestExpectationError);
				}

				SECTION("As lvalue.")
				{
					REQUIRE_THROWS_AS(
						mock(),
						TestExpectationError);
				}

				SECTION("As const rvalue.")
				{
					REQUIRE_THROWS_AS(
						std::move(std::as_const(mock))(),
						TestExpectationError);
				}

				REQUIRE_THAT(
					reporter.no_match_reports(),
					Catch::Matchers::SizeIs(1));
				REQUIRE_THAT(
					reporter.exhausted_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.ok_match_reports(),
					Catch::Matchers::IsEmpty());
				REQUIRE_THAT(
					reporter.unsatisfied_expectations(),
					Catch::Matchers::IsEmpty());
			}

			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::SizeIs(1));
		}
	}
}

TEST_CASE("Mock<void()>::expect_lvalue_call expectes a call with lvalue category.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	SECTION("Succeeds as any lvalue.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_lvalue_call();
			SECTION("As const lvalue.")
			{
				REQUIRE_NOTHROW(std::as_const(mock)());
			}

			SECTION("As lvalue.")
			{
				REQUIRE_NOTHROW(mock());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::IsEmpty());
	}

	SECTION("Fails as any rvalue.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_lvalue_call();

			SECTION("As const rvalue.")
			{
				REQUIRE_THROWS_AS(
					std::move(std::as_const(mock))(),
					TestExpectationError);
			}

			SECTION("As rvalue.")
			{
				REQUIRE_THROWS_AS(
					std::move(mock)(),
					TestExpectationError);
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::SizeIs(1));
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::SizeIs(1));
	}
}

TEST_CASE("Mock<void()>::expect_rvalue_call expectes a call with rvalue category.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	SECTION("Succeeds as any rvalue.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_rvalue_call();
			SECTION("As const rvalue.")
			{
				REQUIRE_NOTHROW(std::move(std::as_const(mock))());
			}

			SECTION("As rvalue.")
			{
				REQUIRE_NOTHROW(std::move(mock)());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::IsEmpty());
	}

	SECTION("Fails as any lvalue.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_rvalue_call();

			SECTION("As const lvalue.")
			{
				REQUIRE_THROWS_AS(
					std::as_const(mock)(),
					TestExpectationError);
			}

			SECTION("As lvalue.")
			{
				REQUIRE_THROWS_AS(
					mock(),
					TestExpectationError);
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::SizeIs(1));
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::SizeIs(1));
	}
}

TEST_CASE("Mock<void()>::expect_const_call expectes a const call.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	SECTION("Succeeds as any const.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_const_call();
			SECTION("As const lvalue.")
			{
				REQUIRE_NOTHROW(std::as_const(mock)());
			}

			SECTION("As const rvalue.")
			{
				REQUIRE_NOTHROW(std::move(std::as_const(mock))());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::IsEmpty());
	}

	SECTION("Fails as any mutable.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_const_call();

			SECTION("As lvalue.")
			{
				REQUIRE_THROWS_AS(
					mock(),
					TestExpectationError);
			}

			SECTION("As rvalue.")
			{
				REQUIRE_THROWS_AS(
					std::move(mock)(),
					TestExpectationError);
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::SizeIs(1));
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::SizeIs(1));
	}
}

TEST_CASE("Mock<void()>::expect_mutable_call expectes a mutable call.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	SECTION("Succeeds as any mutable.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_mutable_call();
			SECTION("As lvalue.")
			{
				REQUIRE_NOTHROW(mock());
			}

			SECTION("As rvalue.")
			{
				REQUIRE_NOTHROW(std::move(mock)());
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::SizeIs(1));
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::IsEmpty());
	}

	SECTION("Fails as any const.")
	{
		{
			ExpectationT expectation = std::as_const(mock).expect_mutable_call();

			SECTION("As const lvalue.")
			{
				REQUIRE_THROWS_AS(
					std::as_const(mock)(),
					TestExpectationError);
			}

			SECTION("As const rvalue.")
			{
				REQUIRE_THROWS_AS(
					std::move(std::as_const(mock))(),
					TestExpectationError);
			}

			REQUIRE_THAT(
				reporter.no_match_reports(),
				Catch::Matchers::SizeIs(1));
			REQUIRE_THAT(
				reporter.exhausted_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.ok_match_reports(),
				Catch::Matchers::IsEmpty());
			REQUIRE_THAT(
				reporter.unsatisfied_expectations(),
				Catch::Matchers::IsEmpty());
		}

		REQUIRE_THAT(
			reporter.unsatisfied_expectations(),
			Catch::Matchers::SizeIs(1));
	}
}

TEST_CASE("Mock<void()>::expect_any_call expects call from any category and constness.", "[mock]")
{
	using ExpectationT = mimicpp::ScopedExpectation<void()>;
	mimicpp::ScopedReporter reporter{};
	mimicpp::Mock<void()> mock{};

	{
		ExpectationT expectation = std::as_const(mock).expect_any_call();

		SECTION("As const lvalue.")
		{
			REQUIRE_NOTHROW(std::as_const(mock)());
		}

		SECTION("As lvalue.")
		{
			REQUIRE_NOTHROW(mock());
		}

		SECTION("As const rvalue.")
		{
			REQUIRE_NOTHROW(std::move(std::as_const(mock))());
		}

		SECTION("As rvalue.")
		{
			REQUIRE_NOTHROW(std::move(mock)());
		}

		REQUIRE_THAT(
			reporter.no_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.exhausted_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.ok_match_reports(),
			Catch::Matchers::SizeIs(1));
	}

	REQUIRE_THAT(
		reporter.unsatisfied_expectations(),
		Catch::Matchers::IsEmpty());
}

//
//TEST_CASE("Mock::expect creates a new expectation.", "[mock]")
//{
//	using SignatureT = void(int, double);
//	mimicpp::ScopedReporter reporter{};
//	mimicpp::Mock<SignatureT> mock{};
//	PolicyFake<SignatureT> configurablePolicy{};
//
//	const auto makeExpectation = [&](auto& m)
//	{
//		return m.expect_call(1, 4.2)
//				| PolicyFacade<SignatureT, std::reference_wrapper<PolicyFake<SignatureT>>, UnwrapReferenceWrapper>{
//					std::ref(configurablePolicy)
//				};
//	};
//
//	SECTION("When matched, gets reported as ok.")
//	{
//		{
//			configurablePolicy.matchResult = mimicpp::call::SubMatchResult{true};
//			mimicpp::ScopedExpectation<SignatureT> expectation = makeExpectation(mock);
//			REQUIRE_NOTHROW(mock(1, 4.2));
//			configurablePolicy.isSatisfied = true; // mark as accepted
//		}
//
//		REQUIRE_THAT(
//			reporter.no_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.exhausted_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.unsatisfied_expectations(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.ok_match_reports(),
//			Catch::Matchers::SizeIs(1));
//	}
//
//	SECTION("When partial matched, gets reported as partial match and unsatisfied.")
//	{
//		{
//			mimicpp::ScopedExpectation<SignatureT> expectation = makeExpectation(mock);
//			REQUIRE_THROWS_AS(
//				mock(42, 4.2),
//				TestExpectationError);
//		}
//
//		REQUIRE_THAT(
//			reporter.no_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.exhausted_match_reports(),
//			Catch::Matchers::SizeIs(1));
//
//		REQUIRE_THAT(
//			reporter.unsatisfied_expectations(),
//			Catch::Matchers::SizeIs(1));
//
//		REQUIRE_THAT(
//			reporter.ok_match_reports(),
//			Catch::Matchers::IsEmpty());
//	}
//
//	SECTION("When not matched, gets reported as no match and unsatisfied.")
//	{
//		{
//			mimicpp::ScopedExpectation<SignatureT> expectation = makeExpectation(mock);
//			REQUIRE_THROWS_AS(
//				std::move(std::as_const(mock))(42, 8.4),
//				TestExpectationError);
//		}
//
//		REQUIRE_THAT(
//			reporter.no_match_reports(),
//			Catch::Matchers::SizeIs(1));
//
//		REQUIRE_THAT(
//			reporter.exhausted_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.unsatisfied_expectations(),
//			Catch::Matchers::SizeIs(1));
//
//		REQUIRE_THAT(
//			reporter.ok_match_reports(),
//			Catch::Matchers::IsEmpty());
//	}
//
//	SECTION("When nothing is invoked, gets reported as unsatisfied.")
//	{
//		{
//			mimicpp::ScopedExpectation<SignatureT> expectation = makeExpectation(mock);
//		}
//
//		REQUIRE_THAT(
//			reporter.no_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.exhausted_match_reports(),
//			Catch::Matchers::IsEmpty());
//
//		REQUIRE_THAT(
//			reporter.unsatisfied_expectations(),
//			Catch::Matchers::SizeIs(1));
//
//		REQUIRE_THAT(
//			reporter.ok_match_reports(),
//			Catch::Matchers::IsEmpty());
//	}
//}
