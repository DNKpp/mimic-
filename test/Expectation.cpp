// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#include "mimic++/Expectation.hpp"
#include "TestReporter.hpp"

#include <functional>
#include <optional>
#include <ranges>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/trompeloeil.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <catch2/matchers/catch_matchers_container_properties.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

namespace
{
	class ExpectationMock final
		: public mimicpp::Expectation<void()>
	{
	public:
		using CallInfoT = mimicpp::call::Info<void()>;
		using MatchResultT = mimicpp::call::MatchResultT;

		MAKE_CONST_MOCK0(is_satisfied, bool(), noexcept override);
		MAKE_CONST_MOCK0(is_saturated, bool(), noexcept override);
		MAKE_CONST_MOCK1(matches, MatchResultT(const CallInfoT&), override);
		MAKE_MOCK1(consume, void(const CallInfoT&), override);
		MAKE_MOCK1(finalize_call, void(const CallInfoT&), override);
	};
}

TEST_CASE(
	"mimicpp::ExpectationCollection collects expectations.",
	"[expectation]"
)
{
	using StorageT = mimicpp::ExpectationCollection<void()>;

	StorageT storage{};
	auto expectation = std::make_shared<ExpectationMock>();

	REQUIRE_NOTHROW(storage.push(expectation));

	ALLOW_CALL(*expectation, is_satisfied())
		.RETURN(true);
	REQUIRE_NOTHROW(storage.remove(expectation));
}

TEST_CASE(
	"mimicpp::ExpectationCollection queries expectation, whether they match the call.",
	"[expectation]"
)
{
	using namespace mimicpp::call;
	using StorageT = mimicpp::ExpectationCollection<void()>;
	using CallInfoT = Info<void()>;
	using trompeloeil::_;

	mimicpp::ScopedReporter reporter{};
	StorageT storage{};
	std::vector<std::shared_ptr<ExpectationMock>> expectations(4);
	for (auto& exp : expectations)
	{
		exp = std::make_shared<ExpectationMock>();
		storage.push(exp);
	}

	const CallInfoT call{
		.params = {},
		.fromUuid = 0,
		.fromCategory = ValueCategory::lvalue,
		.fromConst = false
	};

	SECTION("If a full match is found.")
	{
		trompeloeil::sequence sequence{};
		REQUIRE_CALL(*expectations[0], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});
		REQUIRE_CALL(*expectations[1], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});
		REQUIRE_CALL(*expectations[2], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_OkT{});
		// expectations[3] is never queried
		REQUIRE_CALL(*expectations[2], consume(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence);
		REQUIRE_CALL(*expectations[2], finalize_call(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence);

		REQUIRE_NOTHROW(storage.handle_call(call));
		REQUIRE_THAT(
			reporter.no_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.partial_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.ok_match_reports(),
			Catch::Matchers::SizeIs(1));
	}

	SECTION("If none matches full, but at least one matches partially.")
	{
		const auto [count, result0, result1, result2, result3] = GENERATE(
			(table<std::size_t, MatchResultT, MatchResultT, MatchResultT, MatchResultT>)(
				{
				{1u, MatchResult_PartialT{}, MatchResult_NoT{}, MatchResult_NoT{}, MatchResult_NoT{}},
				{1u, MatchResult_NoT{}, MatchResult_PartialT{}, MatchResult_NoT{}, MatchResult_NoT{}},
				{1u, MatchResult_NoT{}, MatchResult_NoT{}, MatchResult_PartialT{}, MatchResult_NoT{}},
				{1u, MatchResult_NoT{}, MatchResult_NoT{}, MatchResult_NoT{}, MatchResult_PartialT{}},

				{2u, MatchResult_PartialT{}, MatchResult_NoT{}, MatchResult_PartialT{}, MatchResult_NoT{}},
				{2u, MatchResult_NoT{}, MatchResult_PartialT{}, MatchResult_NoT{}, MatchResult_PartialT{}},
				{3u, MatchResult_PartialT{}, MatchResult_NoT{}, MatchResult_PartialT{}, MatchResult_PartialT{}},
				{4u, MatchResult_PartialT{}, MatchResult_PartialT{}, MatchResult_PartialT{}, MatchResult_PartialT{}}
				}));

		trompeloeil::sequence sequence{};
		REQUIRE_CALL(*expectations[0], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(result0);
		REQUIRE_CALL(*expectations[1], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(result1);
		REQUIRE_CALL(*expectations[2], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(result2);
		REQUIRE_CALL(*expectations[3], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(result3);

		REQUIRE_THROWS_AS(
			storage.handle_call(call),
			TestExpectationError);
		REQUIRE_THAT(
			reporter.no_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.partial_match_reports(),
			Catch::Matchers::SizeIs(count));
		REQUIRE_THAT(
			reporter.ok_match_reports(),
			Catch::Matchers::IsEmpty());
	}

	SECTION("If all do not match.")
	{
		trompeloeil::sequence sequence{};
		REQUIRE_CALL(*expectations[0], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});
		REQUIRE_CALL(*expectations[1], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});
		REQUIRE_CALL(*expectations[2], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});
		REQUIRE_CALL(*expectations[3], matches(_))
			.LR_WITH(&_1 == &call)
			.IN_SEQUENCE(sequence)
			.RETURN(MatchResult_NoT{});

		REQUIRE_THROWS_AS(
			storage.handle_call(call),
			TestExpectationError);
		REQUIRE_THAT(
			reporter.no_match_reports(),
			Catch::Matchers::SizeIs(4));
		REQUIRE_THAT(
			reporter.partial_match_reports(),
			Catch::Matchers::IsEmpty());
		REQUIRE_THAT(
			reporter.ok_match_reports(),
			Catch::Matchers::IsEmpty());
	}
}

namespace
{
	template <typename Signature>
	class PolicyMock
	{
	public:
		using CallInfoT = mimicpp::call::Info<Signature>;
		using SubMatchT = mimicpp::call::SubMatchResultT;

		static constexpr bool trompeloeil_movable_mock = true;

		MAKE_CONST_MOCK0(is_satisfied, bool(), noexcept);
		MAKE_CONST_MOCK0(is_saturated, bool(), noexcept);
		MAKE_CONST_MOCK1(matches, SubMatchT(const CallInfoT&), noexcept);
		MAKE_MOCK1(consume, void(const CallInfoT&), noexcept);
	};

	template <typename Signature>
	class PolicyFake
	{
	public:
		using CallInfoT = mimicpp::call::Info<Signature>;
		using SubMatchT = mimicpp::call::SubMatchResultT;

		[[nodiscard]]
		static constexpr bool is_satisfied() noexcept
		{
			return false;
		}

		[[nodiscard]]
		static constexpr bool is_saturated() noexcept
		{
			return false;
		}

		[[nodiscard]]
		static constexpr SubMatchT matches(const CallInfoT& call) noexcept
		{
			return matches(call);
		}

		static constexpr void consume(const CallInfoT& call) noexcept
		{
		}
	};

	template <typename Signature, typename Policy, typename Projection>
	// enable, when trompeloeil fully supports movable mocks
	//requires mimicpp::expectation_policy_for<
	//	std::remove_cvref_t<std::invoke_result_t<Projection, Policy>>,
	//	Signature>
	class PolicyFacade
	{
	public:
		using CallT = mimicpp::call::Info<Signature>;
		using SubMatchT = mimicpp::call::SubMatchResultT;

		Policy policy{};
		Projection projection{};

		[[nodiscard]]
		constexpr bool is_satisfied() const noexcept
		{
			return std::invoke(projection, policy)
				.is_satisfied();
		}

		[[nodiscard]]
		constexpr bool is_saturated() const noexcept
		{
			return std::invoke(projection, policy)
				.is_saturated();
		}

		[[nodiscard]]
		constexpr SubMatchT matches(const CallT& call) const noexcept
		{
			return std::invoke(projection, policy)
				.matches(call);
		}

		constexpr void consume(const CallT& call) noexcept
		{
			std::invoke(projection, policy)
				.consume(call);
		}
	};

	class UnwrapReferenceWrapper
	{
	public:
		template <typename T>
		constexpr T& operator ()(const std::reference_wrapper<T> ref) const noexcept
		{
			return ref.get();
		}
	};

	template <typename Signature>
	class FinalizerFake
	{
	public:
		using CallInfoT = mimicpp::call::Info<Signature>;
		using ReturnT = mimicpp::signature_return_type_t<Signature>;

		class Exception
		{
		};

		static ReturnT finalize_call(const CallInfoT& call)
		{
			throw Exception{};
		}
	};

	template <typename Signature>
	class FinalizerMock
	{
	public:
		using CallInfoT = mimicpp::call::Info<Signature>;
		using ReturnT = mimicpp::signature_return_type_t<Signature>;

		MAKE_MOCK1(finalize_call, ReturnT(const CallInfoT&));
	};

	template <typename Signature, typename Policy, typename Projection>
	// enable, when trompeloeil fully supports movable mocks
	//requires mimicpp::finalize_policy_for<
	//	std::remove_cvref_t<std::invoke_result_t<Projection, Policy>>,
	//	Signature>
	class FinalizerFacade
	{
	public:
		using CallInfoT = mimicpp::call::Info<Signature>;
		using ReturnT = mimicpp::signature_return_type_t<Signature>;

		Policy policy{};
		Projection projection{};

		[[nodiscard]]
		constexpr ReturnT finalize_call(const CallInfoT& call)
		{
			return std::invoke(projection, policy)
				.finalize_call(call);
		}
	};
}

TEMPLATE_TEST_CASE_SIG(
	"mimicpp::expectation_policy_for determines, whether the type can be used in combination with the given signature.",
	"[expectation]",
	((bool expected, typename Policy, typename Signature), expected, Policy, Signature),
	(false, PolicyFake<void()>, int()),	// incompatible return type
	(false, PolicyFake<void()>, void(int)),	// incompatible param
	(true, PolicyFake<void()>, void()),
	(true, PolicyFacade<void(), std::reference_wrapper<PolicyFake<void()>>, UnwrapReferenceWrapper>, void())
)
{
	STATIC_REQUIRE(expected == mimicpp::expectation_policy_for<Policy, Signature>);
}

TEMPLATE_TEST_CASE_SIG(
	"mimicpp::finalize_policy_for determines, whether the type can be used in combination with the given signature.",
	"[expectation]",
	((bool expected, typename Policy, typename Signature), expected, Policy, Signature),
	(false, FinalizerFake<void()>, int()),	// incompatible return type
	(false, FinalizerFake<void()>, void(int)),	// incompatible param
	(true, FinalizerFake<void()>, void()),
	(true, FinalizerFacade<void(), std::reference_wrapper<FinalizerFake<void()>>, UnwrapReferenceWrapper>, void())
)
{
	STATIC_REQUIRE(expected == mimicpp::finalize_policy_for<Policy, Signature>);
}

namespace
{
	const std::array allSubMatchResultAlternatives = std::to_array<mimicpp::call::SubMatchResultT>(
		{
			mimicpp::call::SubMatchResult_NoT{},
			mimicpp::call::SubMatchResult_PartialT{},
			mimicpp::call::SubMatchResult_OkT{}
		});

	class CallMatchCategoryMatcher final
		: public Catch::Matchers::MatcherGenericBase
	{
	public:
		using CategoryT = mimicpp::call::MatchCategory;

		[[nodiscard]]
		explicit CallMatchCategoryMatcher(const CategoryT category) noexcept
			: m_Category{category}
		{
		}

		bool match(const auto& result) const
		{
			return m_Category == std::visit(
						[](const auto& inner) { return inner.value; },
						result);
		}

		std::string describe() const override
		{
			return std::format(
				"matches category: {}",
				m_Category);
		}

	private:
		CategoryT m_Category;
	};

	[[nodiscard]]
	CallMatchCategoryMatcher matches_category(const mimicpp::call::MatchCategory category) noexcept
	{
		return CallMatchCategoryMatcher{category};
	}

	[[nodiscard]]
	CallMatchCategoryMatcher matches_category_of(const auto& result) noexcept
	{
		return matches_category(
			std::visit(
				[](const auto& inner) { return inner.value; },
				result));
	}

	template <std::ranges::input_range Range>
	[[nodiscard]]
	CallMatchCategoryMatcher matches_least_category_of(Range&& results) noexcept
	{
		auto categories = std::forward<Range>(results)
						| std::views::transform(
							[](const auto& el) noexcept
							{
								return std::visit(
									[](const auto& inner) noexcept { return inner.value; },
									el);
							});

		return matches_category(
			std::ranges::min(categories));
	}
}

TEMPLATE_TEST_CASE(
	"mimicpp::BasicExpectation can be extended with an arbitrary policy amount.",
	"[expectation]",
	void(),
	int()
)
{
	using trompeloeil::_;
	using FinalizerT = FinalizerFake<TestType>;
	using PolicyMockT = PolicyMock<TestType>;
	using PolicyRefT = PolicyFacade<TestType, std::reference_wrapper<PolicyMock<TestType>>, UnwrapReferenceWrapper>;
	using CallInfoT = mimicpp::call::Info<TestType>;

	const CallInfoT call{
		.params = {},
		.fromUuid = 0,
		.fromCategory = mimicpp::call::ValueCategory::lvalue,
		.fromConst = false
	};

	SECTION("With no policies at all.")
	{
		mimicpp::BasicExpectation<TestType, FinalizerT> expectation{
			FinalizerT{}
		};

		REQUIRE(std::as_const(expectation).is_satisfied());
		REQUIRE(!std::as_const(expectation).is_saturated());
		REQUIRE(std::holds_alternative<mimicpp::call::MatchResult_OkT>(std::as_const(expectation).matches(call)));
		REQUIRE_NOTHROW(expectation.consume(call));
	}

	SECTION("With one policy.")
	{
		PolicyMockT policy{};
		mimicpp::BasicExpectation<TestType, FinalizerT, PolicyRefT> expectation{
			FinalizerT{},
			PolicyRefT{std::ref(policy)}
		};

		const bool isSatisfied = GENERATE(false, true);
		REQUIRE_CALL(policy, is_satisfied())
			.RETURN(isSatisfied);
		REQUIRE(isSatisfied == std::as_const(expectation).is_satisfied());

		const bool isSaturated = GENERATE(false, true);
		REQUIRE_CALL(policy, is_saturated())
			.RETURN(isSaturated);
		REQUIRE(isSaturated == std::as_const(expectation).is_saturated());

		const auto matchesResult = GENERATE(from_range(allSubMatchResultAlternatives));
		REQUIRE_CALL(policy, matches(_))
			.LR_WITH(&_1 == &call)
			.RETURN(matchesResult);
		REQUIRE_THAT(
			std::as_const(expectation).matches(call),
			matches_category_of(matchesResult));

		REQUIRE_CALL(policy, consume(_))
			.LR_WITH(&_1 == &call);
		expectation.consume(call);
	}

	SECTION("With two policies.")
	{
		PolicyMockT policy1{};
		PolicyMockT policy2{};
		mimicpp::BasicExpectation<TestType, FinalizerT, PolicyRefT, PolicyRefT> expectation{
			FinalizerT{},
			PolicyRefT{std::ref(policy1)},
			PolicyRefT{std::ref(policy2)}
		};

		SECTION("When calling is_satisfied()")
		{
			const bool isSatisfied1 = GENERATE(false, true);
			const bool isSatisfied2 = GENERATE(false, true);
			const bool expectedIsSatisfied = isSatisfied1 && isSatisfied2;
			REQUIRE_CALL(policy1, is_satisfied())
				.RETURN(isSatisfied1);
			auto policy2Expectation = std::invoke(
				[&]() -> std::unique_ptr<trompeloeil::expectation>
				{
					if (isSatisfied1)
					{
						return NAMED_REQUIRE_CALL(policy2, is_satisfied())
							.RETURN(isSatisfied2);
					}
					return nullptr;
				});

			REQUIRE(expectedIsSatisfied == std::as_const(expectation).is_satisfied());
		}

		SECTION("When calling is_saturated()")
		{
			const bool isSaturated1 = GENERATE(false, true);
			const bool isSaturated2 = GENERATE(false, true);
			const bool expectedIsSaturated = isSaturated1 || isSaturated2;
			REQUIRE_CALL(policy1, is_saturated())
				.RETURN(isSaturated1);
			auto policy2Expectation = std::invoke(
				[&]() -> std::unique_ptr<trompeloeil::expectation>
				{
					if (!isSaturated1)
					{
						return NAMED_REQUIRE_CALL(policy2, is_saturated())
							.RETURN(isSaturated2);
					}
					return nullptr;
				});

			REQUIRE(expectedIsSaturated == std::as_const(expectation).is_saturated());
		}

		SECTION("When calling matches()")
		{
			const auto matchResult1 = GENERATE(from_range(allSubMatchResultAlternatives));
			const auto matchResult2 = GENERATE(from_range(allSubMatchResultAlternatives));

			REQUIRE_CALL(policy1, matches(_))
				.LR_WITH(&_1 == &call)
				.RETURN(matchResult1);
			REQUIRE_CALL(policy2, matches(_))
				.LR_WITH(&_1 == &call)
				.RETURN(matchResult2);

			REQUIRE_THAT(
				std::as_const(expectation).matches(call),
				matches_least_category_of(std::array{matchResult1, matchResult2}));
		}

		SECTION("When calling consume()")
		{
			REQUIRE_CALL(policy1, consume(_))
			   .LR_WITH(&_1 == &call);
			REQUIRE_CALL(policy2, consume(_))
				.LR_WITH(&_1 == &call);
			expectation.consume(call);
		}
	}
}

TEMPLATE_TEST_CASE(
	"mimicpp::BasicExpectation finalizer can be exchanged.",
	"[expectation]",
	void(),
	int()
)
{
	using trompeloeil::_;
	using SignatureT = TestType;
	using FinalizerT = FinalizerMock<SignatureT>;
	using FinalizerRefT = FinalizerFacade<SignatureT, std::reference_wrapper<FinalizerT>, UnwrapReferenceWrapper>;
	using CallInfoT = mimicpp::call::Info<SignatureT>;

	const CallInfoT call{
		.params = {},
		.fromUuid = 0,
		.fromCategory = mimicpp::call::ValueCategory::lvalue,
		.fromConst = false
	};

	FinalizerT finalizer{};
	mimicpp::BasicExpectation<SignatureT, FinalizerRefT> expectation{
		std::ref(finalizer)
	};

	class Exception
	{
	};
	REQUIRE_CALL(finalizer, finalize_call(_))
		.LR_WITH(&_1 == &call)
		.THROW(Exception{});
	REQUIRE_THROWS_AS(expectation.finalize_call(call), Exception);
}

TEST_CASE("ScopedExpectation is a non-copyable, but movable type.")
{
	using ScopedExpectationT = mimicpp::ScopedExpectation<void()>;

	STATIC_REQUIRE(!std::is_copy_constructible_v<ScopedExpectationT>);
	STATIC_REQUIRE(!std::is_copy_assignable_v<ScopedExpectationT>);
	STATIC_REQUIRE(std::is_move_constructible_v<ScopedExpectationT>);
	STATIC_REQUIRE(std::is_move_assignable_v<ScopedExpectationT>);
}

TEST_CASE(
	"ScopedExpectation handles expectation lifetime and reports when not satisfied.",
	"[expectation]")
{
	using trompeloeil::_;
	using PolicyMockT = PolicyMock<void()>;
	using PolicyRefT = PolicyFacade<void(), std::reference_wrapper<PolicyMock<void()>>, UnwrapReferenceWrapper>;
	using FinalizerT = FinalizerFake<void()>;
	using ExpectationT = mimicpp::BasicExpectation<void(), FinalizerT, PolicyRefT>;
	using ScopedExpectationT = mimicpp::ScopedExpectation<void()>;
	using CollectionT = mimicpp::ExpectationCollection<void()>;

	auto collection = std::make_shared<CollectionT>();

	PolicyMockT policy{};
	std::optional<ScopedExpectationT> expectation{
		std::in_place,
		collection,
		std::make_unique<ExpectationT>(FinalizerT{}, std::ref(policy))
	};

	SECTION("When calling is_satisfied()")
	{
		const bool isSatisfied = GENERATE(false, true);
		REQUIRE_CALL(policy, is_satisfied())
			.RETURN(isSatisfied);
		REQUIRE(isSatisfied == std::as_const(expectation)->is_satisfied());
	}

	SECTION("When calling is_saturated()")
	{
		const bool isSaturated = GENERATE(false, true);
		REQUIRE_CALL(policy, is_saturated())
			.RETURN(isSaturated);
		REQUIRE(isSaturated == std::as_const(expectation)->is_saturated());
	}

	SECTION("When ScopedExpectation is moved.")
	{
		ScopedExpectationT otherExpectation = *std::move(expectation);

		SECTION("When calling is_satisfied()")
		{
			const bool isSatisfied = GENERATE(false, true);
			REQUIRE_CALL(policy, is_satisfied())
				.RETURN(isSatisfied);
			REQUIRE(isSatisfied == std::as_const(otherExpectation).is_satisfied());
			REQUIRE_THROWS_AS(std::as_const(expectation)->is_satisfied(), std::runtime_error);
		}

		SECTION("When calling is_saturated()")
		{
			const bool isSaturated = GENERATE(false, true);
			REQUIRE_CALL(policy, is_saturated())
				.RETURN(isSaturated);
			REQUIRE(isSaturated == std::as_const(otherExpectation).is_saturated());
			REQUIRE_THROWS_AS(std::as_const(expectation)->is_saturated(), std::runtime_error);
		}

		SECTION("And then move assigned.")
		{
			expectation = std::move(otherExpectation);

			SECTION("When calling is_satisfied()")
			{
				const bool isSatisfied = GENERATE(false, true);
				REQUIRE_CALL(policy, is_satisfied())
					.RETURN(isSatisfied);
				REQUIRE(isSatisfied == std::as_const(expectation)->is_satisfied());
				REQUIRE_THROWS_AS(std::as_const(otherExpectation).is_satisfied(), std::runtime_error);
			}

			SECTION("When calling is_saturated()")
			{
				const bool isSaturated = GENERATE(false, true);
				REQUIRE_CALL(policy, is_saturated())
					.RETURN(isSaturated);
				REQUIRE(isSaturated == std::as_const(expectation)->is_saturated());
				REQUIRE_THROWS_AS(std::as_const(otherExpectation).is_saturated(), std::runtime_error);
			}

			// just move back, so we can unify the cleanup process
			otherExpectation = *std::move(expectation);
		}

		SECTION("And then self move assigned.")
		{
			otherExpectation = std::move(otherExpectation);

			SECTION("When calling is_satisfied()")
			{
				const bool isSatisfied = GENERATE(false, true);
				REQUIRE_CALL(policy, is_satisfied())
					.RETURN(isSatisfied);
				REQUIRE(isSatisfied == std::as_const(otherExpectation).is_satisfied());
			}

			SECTION("When calling is_saturated()")
			{
				const bool isSaturated = GENERATE(false, true);
				REQUIRE_CALL(policy, is_saturated())
					.RETURN(isSaturated);
				REQUIRE(isSaturated == std::as_const(otherExpectation).is_saturated());
			}
		}

		// just move back, so we can unify the cleanup process
		expectation = std::move(otherExpectation);
	}

	// collection asserts on that in remove
	ALLOW_CALL(policy, is_satisfied())
		.RETURN(true);
	expectation.reset();
}
