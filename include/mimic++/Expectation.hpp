// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#ifndef MIMICPP_EXPECTATION_HPP
#define MIMICPP_EXPECTATION_HPP

#pragma once

#include "Call.hpp"

#include <cassert>
#include <concepts>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace mimicpp
{
	template <typename Signature>
	class Expectation
	{
	public:
		using CallT = Call<Signature>;

		virtual ~Expectation() = default;

		[[nodiscard]]
		Expectation() = default;

		Expectation(const Expectation&) = delete;
		Expectation& operator =(const Expectation&) = delete;
		Expectation(Expectation&&) = delete;
		Expectation& operator =(Expectation&&) = delete;

		[[nodiscard]]
		virtual bool is_satisfied() const noexcept = 0;

		[[nodiscard]]
		virtual bool is_saturated() const noexcept = 0;

		[[nodiscard]]
		virtual bool matches(const CallT& call) const noexcept = 0;
		virtual void consume(const CallT& call) noexcept = 0;
	};

	template <typename Signature>
	class ExpectationCollection
	{
	public:
		using CallT = Call<Signature>;
		using ExpectationT = Expectation<Signature>;

		~ExpectationCollection() = default;

		[[nodiscard]]
		ExpectationCollection() = default;

		ExpectationCollection(const ExpectationCollection&) = delete;
		ExpectationCollection& operator =(const ExpectationCollection&) = delete;

		[[nodiscard]]
		ExpectationCollection(ExpectationCollection&&) = default;
		ExpectationCollection& operator =(ExpectationCollection&&) = default;

		void push(std::shared_ptr<ExpectationT> expectation)
		{
			const std::scoped_lock lock{m_ExpectationsMx};

			assert(
				std::ranges::find(m_Expectations, expectation) == std::ranges::end(m_Expectations)
				&& "Expectation already belongs to this storage.");

			m_Expectations.emplace_back(std::move(expectation));
		}

		void remove(std::shared_ptr<ExpectationT> expectation) noexcept
		{
			const std::scoped_lock lock{m_ExpectationsMx};

			auto iter = std::ranges::find(m_Expectations, expectation);
			assert(iter != std::ranges::end(m_Expectations) && "Expectation does not belong to this storage.");
			assert((*iter)->is_satisfied() && "Expectation is unsatisfied.");
			m_Expectations.erase(iter);
		}

		[[nodiscard]]
		bool consume(const CallT& call) noexcept
		{
			const std::scoped_lock lock{m_ExpectationsMx};

			if (const auto iter = std::ranges::find_if(
				m_Expectations,
				[&](const auto& exp)
				{
					return !exp->is_saturated()
							&& exp->matches(call);
				});
				iter != std::ranges::end(m_Expectations))
			{
				(*iter)->consume(call);
				return true;
			}

			return false;
		}

	private:
		std::vector<std::shared_ptr<ExpectationT>> m_Expectations{};
		std::mutex m_ExpectationsMx{};
	};

	template <typename T, typename Signature>
	concept expectation_policy_for = std::movable<T>
									&& std::is_destructible_v<T>
									&& std::same_as<T, std::remove_cvref_t<T>>
									&& requires(T& policy, const Call<Signature>& call)
									{
										{ policy.is_satisfied() } noexcept -> std::convertible_to<bool>;
										{ policy.is_saturated() } noexcept -> std::convertible_to<bool>;
										{ policy.matches(call) } noexcept -> std::convertible_to<bool>;
										{ policy.consume(call) } noexcept;
									};

	template <typename Signature, expectation_policy_for<Signature>... Policies>
	class BasicExpectation final
		: public Expectation<Signature>
	{
	public:
		using PolicyListT = std::tuple<Policies...>;
		using CallT = Call<Signature>;

		template <typename... PolicyArgs>
			requires std::constructible_from<PolicyListT, PolicyArgs...>
		constexpr explicit BasicExpectation(
			PolicyArgs&&... args
		) noexcept((std::is_nothrow_constructible_v<Policies, PolicyArgs> && ...))
			: m_Policies{std::forward<PolicyArgs>(args)...}
		{
		}

		template <typename... OtherPolicies, typename PolicyArg>
			requires std::constructible_from<PolicyListT, OtherPolicies&&..., PolicyArg>
		constexpr explicit BasicExpectation(BasicExpectation<Signature, OtherPolicies...>&& other, PolicyArg&& arg)
			: m_Policies{
				std::tuple_cat(
					std::move(other).m_Policies,
					std::forward_as_tuple(std::forward<PolicyArg>(arg)))
			}
		{
		}

		[[nodiscard]]
		constexpr bool is_satisfied() const noexcept override
		{
			return std::apply(
				[](const auto&... policies) noexcept
				{
					return (... && policies.is_satisfied());
				},
				m_Policies);
		}

		[[nodiscard]]
		constexpr bool is_saturated() const noexcept override
		{
			return std::apply(
				[](const auto&... policies) noexcept
				{
					return (... || policies.is_saturated());
				},
				m_Policies);
		}

		[[nodiscard]]
		constexpr bool matches(const CallT& call) const noexcept override
		{
			return std::apply(
				[&](const auto&... policies) noexcept
				{
					return (... && policies.matches(call));
				},
				m_Policies);
		}

		constexpr void consume(const CallT& call) noexcept override
		{
			std::apply(
				[&](auto&... policies) noexcept
				{
					(..., policies.consume(call));
				},
				m_Policies);
		}

	private:
		PolicyListT m_Policies;
	};
}

#endif
