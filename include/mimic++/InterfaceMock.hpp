// //          Copyright Dominic (DNKpp) Koepke 2024 - 2024.
// // Distributed under the Boost Software License, Version 1.0.
// //    (See accompanying file LICENSE_1_0.txt or copy at
// //          https://www.boost.org/LICENSE_1_0.txt)

#ifndef MIMICPP_INTERFACE_MOCK_HPP
#define MIMICPP_INTERFACE_MOCK_HPP

#pragma once

#include "mimic++/Fwd.hpp"
#include "mimic++/Mock.hpp"

#include <type_traits>
#include <utility>

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES interfaces
	 * \ingroup MOCK
	 * \brief Contains utility to simplify interface mocking.
	 */

	/**
	 * \defgroup MOCK_INTERFACES_DETAIL detail
	 * \ingroup MOCK_INTERFACES
	 */

	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_STRIP_PARENS strip_parens
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Removes an enclosing pair of (), if present.
	 */
}

/**
 * \brief Removes an enclosing pair of (), if present.
 * \ingroup MOCK_INTERFACES_DETAIL_STRIP_PARENS
 * \param x token
 * \see Inspired by https://stackoverflow.com/a/62984543
 */
#define MIMICPP_DETAIL_STRIP_PARENS(x) MIMICPP_DETAIL_STRIP_PARENS_OUTER(MIMICPP_DETAIL_STRIP_PARENS_INNER x)

/**
 * \brief Black-magic.
 * \ingroup MOCK_INTERFACES_DETAIL_STRIP_PARENS
 */
#define MIMICPP_DETAIL_STRIP_PARENS_INNER(...) MIMICPP_DETAIL_STRIP_PARENS_INNER __VA_ARGS__

/**
 * \brief Black-magic.
 * \ingroup MOCK_INTERFACES_DETAIL_STRIP_PARENS
 */
#define MIMICPP_DETAIL_STRIP_PARENS_OUTER(...) MIMICPP_DETAIL_STRIP_PARENS_OUTER_(__VA_ARGS__)

/**
 * \brief Black-magic.
 * \ingroup MOCK_INTERFACES_DETAIL_STRIP_PARENS
 */
#define MIMICPP_DETAIL_STRIP_PARENS_OUTER_(...) MIMICPP_DETAIL_STRIP_PARENS_STRIPPED_ ## __VA_ARGS__

/**
 * \brief Swallows the leftover token.
 * \ingroup MOCK_INTERFACES_DETAIL_STRIP_PARENS
 */
#define MIMICPP_DETAIL_STRIP_PARENS_STRIPPED_MIMICPP_DETAIL_STRIP_PARENS_INNER

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_FOR_EACH for_each
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief This is an implementation of a for-loop for the preprocessor.
	 * \details This solution is highly inspired by the blog-article of David Mazieres.
	 * He does a very good job in explaining the dark corners of the macro language, but even now, I do not
	 * fully understand how this works. Either way, thank you very much!
	 * \see https://www.scs.stanford.edu/~dm/blog/va-opt.html
	 * \details All macros in this group are required to make that work.
	 */
}

/**
 * \brief Pastes a pair of parentheses.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_PARENS ()

/**
 * \brief Pastes a ,.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_COMMA_DELIMITER() ,

/**
 * \brief Pastes nothing.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_NO_DELIMITER()

/**
 * \brief Pastes all arguments as provided.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_IDENTITY(...) __VA_ARGS__

/**
 * \brief Part of the fake recursion.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_EXPAND(...) MIMICPP_DETAIL_EXPAND3(MIMICPP_DETAIL_EXPAND3(MIMICPP_DETAIL_EXPAND3(MIMICPP_DETAIL_EXPAND3(__VA_ARGS__))))

/**
 * \brief Part of the fake recursion.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_EXPAND3(...) MIMICPP_DETAIL_EXPAND2(MIMICPP_DETAIL_EXPAND2(MIMICPP_DETAIL_EXPAND2(MIMICPP_DETAIL_EXPAND2(__VA_ARGS__))))

/**
 * \brief Part of the fake recursion.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_EXPAND2(...) MIMICPP_DETAIL_EXPAND1(MIMICPP_DETAIL_EXPAND1(MIMICPP_DETAIL_EXPAND1(MIMICPP_DETAIL_EXPAND1(__VA_ARGS__))))

/**
 * \brief Part of the fake recursion.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_EXPAND1(...) __VA_ARGS__

/**
 * \brief Calls the given macro with all other arguments.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 * \param macro Macro to be called.
 * \param sequence First argument.
 * \param Others Accepts arbitrary arguments and forwards them.
 */
#define MIMICPP_DETAIL_FOR_EACH_EXT_INDIRECT(macro, sequence, ...) macro(sequence, __VA_ARGS__)

/**
 * \brief The starting point of the for-each implementation.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 * \param macro The strategy to be executed.
 * \param token A token, which will be expanded for each element.
 * \param delimiter The delimiter, which will added between element.
 * \param projection_macro The projection for the current element.
 * \param bound Addition data, which will be added to the call arguments.
 *
 * \detail This is a very versatile implementation for the for-loop.
 *
 * During the development, it was necessary to generate unique names for function parameters, which I could also directly refer to.
 * That was the reason, why I've added the ``token`` argument. The first element will simply be called with the ``token`` content, but the second
 * with twice the token content and so on. It's ok, to provide an empty argument.
 */
#define MIMICPP_DETAIL_FOR_EACH_EXT(macro, token, delimiter, projection_macro, bound, ...) \
  __VA_OPT__(MIMICPP_DETAIL_EXPAND(MIMICPP_DETAIL_FOR_EACH_EXT_HELPER(macro, token, token, delimiter, projection_macro, bound, __VA_ARGS__)))

/**
 * \brief Black-magic.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_DETAIL_FOR_EACH_EXT_HELPER(macro, token, sequence, delimiter, projection_macro, bound, a1, ...)												\
  MIMICPP_DETAIL_FOR_EACH_EXT_INDIRECT(macro, sequence, MIMICPP_DETAIL_STRIP_PARENS(bound), projection_macro(a1))											\
  __VA_OPT__(delimiter() MIMICPP_FOR_EACH_EXT_AGAIN MIMICPP_DETAIL_PARENS (macro, token, sequence##token, delimiter, projection_macro, bound, __VA_ARGS__))

/**
 * \brief Black-magic.
 * \ingroup MOCK_INTERFACES_DETAIL_FOR_EACH
 */
#define MIMICPP_FOR_EACH_EXT_AGAIN() MIMICPP_DETAIL_FOR_EACH_EXT_HELPER

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_MAKE_SIGNATURE_LIST make_signature_list
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Converts all given arguments to a signature.
	 */
}

/**
 * \brief Converts the given information to a single signature.
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_SIGNATURE_LIST
 * \param sequence Unused.
 * \param bound_data Unused.
 * \param ret The return type.
 * \param param_type_list The parameter types.
 * \param specs Additional specs (e.g. ``const``, ``noexcept``).
 */
#define MIMICPP_DETAIL_MAKE_SIGNATURE(sequence, bound_data, ret, param_type_list, specs, ...) ret param_type_list specs

/**
 * \brief Converts all given arguments to a signature list (not enclosed by parentheses).
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_SIGNATURE_LIST
 */
#define MIMICPP_DETAIL_MAKE_SIGNATURE_LIST(...) \
	MIMICPP_DETAIL_FOR_EACH_EXT(				\
		MIMICPP_DETAIL_MAKE_SIGNATURE,			\
		,										\
		MIMICPP_DETAIL_COMMA_DELIMITER,			\
		MIMICPP_DETAIL_STRIP_PARENS,			\
		,										\
		__VA_ARGS__)

/**
 * \brief Creates a mimicpp::Mock object for the given signatures.
 * \ingroup MOCK_INTERFACES_DETAIL
 * \param mock_name The mock name.
 * \param signatures The given signatures. Enclosing parentheses will be stripped.
 */
#define MIMICPP_DETAIL_MAKE_OVERLOADED_MOCK(mock_name, signatures)	\
	::mimicpp::Mock< MIMICPP_DETAIL_STRIP_PARENS(signatures) > mock_name{}

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_MAKE_PARAM_LIST make_param_list
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Converts all given arguments to a parameter-list.
	 */
}

/**
 * \brief Converts the given information to a single parameter.
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_PARAM_LIST
 * \param sequence A unique sequence, which will be appended to the parameter name (as suffix).
 * \param bound_data Unused.
 * \param type The type of the parameter. Enclosing parentheses will be stripped.
 */
#define MIMICPP_DETAIL_MAKE_PARAM(sequence, bound_data, type) MIMICPP_DETAIL_STRIP_PARENS(type) arg_##sequence

/**
 * \brief Converts all given arguments to a parameter-list (not enclosed by parentheses).
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_PARAM_LIST
 */
#define MIMICPP_DETAIL_MAKE_PARAM_LIST(...) \
	MIMICPP_DETAIL_FOR_EACH_EXT(			\
		MIMICPP_DETAIL_MAKE_PARAM,			\
		i,									\
		MIMICPP_DETAIL_COMMA_DELIMITER,		\
		MIMICPP_DETAIL_IDENTITY,			\
		,									\
		__VA_ARGS__)

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_FORWARD_ARGS forward_args
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Creates ``std::forward`` calls for each given argument (not enclosed by parentheses).
	 */
}

/**
 * \brief Creates a ``std::forward`` call for the given argument.
 * \ingroup MOCK_INTERFACES_DETAIL_FORWARD_ARGS
 * \param sequence A unique sequence, which will be appended to the parameter name (as suffix).
 * \param bound_data Unused.
 * \param type The type of the parameter. Enclosing parentheses will be stripped.
 */
#define MIMICPP_DETAIL_FORWARD_ARG(sequence, bound_data, type) \
	::std::forward<::std::add_rvalue_reference_t<MIMICPP_DETAIL_STRIP_PARENS(type)>>(arg_##sequence)

/**
 * \brief Creates ``std::forward`` calls for each given argument (not enclosed by parentheses).
 * \ingroup MOCK_INTERFACES_DETAIL_FORWARD_ARGS
 */
#define MIMICPP_DETAIL_FORWARD_ARGS(...)	\
	__VA_OPT__(,)							\
	MIMICPP_DETAIL_FOR_EACH_EXT(			\
		MIMICPP_DETAIL_FORWARD_ARG,			\
		i,									\
		MIMICPP_DETAIL_COMMA_DELIMITER,		\
		MIMICPP_DETAIL_IDENTITY,			\
		,									\
		__VA_ARGS__)

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_MAKE_OVERLOAD_INFOS make_overload_infos
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Related functions for MIMICPP_ADD_OVERLOAD.
	 */
}

/**
 * \brief Base overload, extending the overload info (enclosed by parentheses).
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_OVERLOAD_INFOS
 * \param ret The return type.
 * \param param_type_list The parameter types.
 * \param specs An optional parameter for categories (e.g. ``const``, ``noexcept``, etc.).
 */
#define MIMICPP_DETAIL_MAKE_OVERLOAD_INFOS_SPECS(ret, param_type_list, specs, ...) \
	(ret, param_type_list, specs, \
		(MIMICPP_DETAIL_MAKE_PARAM_LIST(MIMICPP_DETAIL_STRIP_PARENS(param_type_list))), \
		(MIMICPP_DETAIL_FORWARD_ARGS(MIMICPP_DETAIL_STRIP_PARENS(param_type_list))))

/**
 * \brief Simple overload, extending the overload info (enclosed by parentheses).
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_OVERLOAD_INFOS
 * \param ret The return type.
 * \param param_type_list The parameter types.
 */
#define MIMICPP_DETAIL_MAKE_OVERLOAD_INFOS_BASIC(ret, param_type_list, ...) \
	MIMICPP_DETAIL_MAKE_OVERLOAD_INFOS_SPECS(ret, param_type_list, )

/**
 * \brief Selects the correct overload, depending on the number of arguments.
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_OVERLOAD_INFOS
 * \see For an explanation of that pattern: https://stackoverflow.com/a/16683147
 */
#define MIMICPP_DETAIL_SELECT_MAKE_OVERLOAD_INFOS(_1, N, ...) N

/**
 * \brief Adds an overload to an interface mock. Used only in combination with \ref MIMICPP_MOCK_OVERLOADED_METHOD.
 * \ingroup MOCK_INTERFACES
 * \param ret The return type.
 * \param param_type_list The parameter types.
 * \param Specs An optional parameter for categories (e.g. ``const``, ``noexcept``, etc.).
 */
#define MIMICPP_ADD_OVERLOAD(ret, param_type_list, ...)								\
	MIMICPP_DETAIL_SELECT_MAKE_OVERLOAD_INFOS(										\
		__VA_ARGS__,																\
		MIMICPP_DETAIL_MAKE_OVERLOAD_INFOS_SPECS,									\
		MIMICPP_DETAIL_MAKE_OVERLOAD_INFOS_BASIC)(ret, param_type_list, __VA_ARGS__)

namespace mimicpp
{
	/**
	 * \defgroup MOCK_INTERFACES_DETAIL_MAKE_METHOD_OVERRIDES make_method_overrides
	 * \ingroup MOCK_INTERFACES_DETAIL
	 * \brief Creates all required override overloads.
	 */
}

/**
 * \brief Create a single overload for the given information.
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_METHOD_OVERRIDES
 * \param ignore Ignored
 * \param mock_name The mock name.
 * \param fn_name The function name.
 * \param ret The return type.
 * \param param_type_list The parameter types.
 * \param specs Additional specifiers (e.g. ``const``, ``noexcept``, etc.).
 * \param param_list Enclosed parameter list.
 * \param forward_list Enclosed forward statements.
 */
#define MIMICPP_DETAIL_MAKE_METHOD_OVERRIDE(ignore, mock_name, fn_name, ret, param_type_list, specs, param_list, forward_list, ...)	\
	inline MIMICPP_DETAIL_STRIP_PARENS(ret) fn_name param_list MIMICPP_DETAIL_STRIP_PARENS(specs) override 							\
	{																																\
		return ::std::invoke(																										\
			mock_name																												\
			MIMICPP_DETAIL_STRIP_PARENS(forward_list));																				\
	}

/**
 * \brief Creates all overloads for a specific function as overrides.
 * \ingroup MOCK_INTERFACES_DETAIL_MAKE_METHOD_OVERRIDES
 * \param mock_name The mock name.
 * \param fn_name The function name to be overloaded.
 */
#define MIMICPP_DETAIL_MAKE_METHOD_OVERRIDES(mock_name, fn_name, ...)	\
	MIMICPP_DETAIL_FOR_EACH_EXT(										\
		MIMICPP_DETAIL_MAKE_METHOD_OVERRIDE,							\
		,																\
		MIMICPP_DETAIL_NO_DELIMITER,									\
		MIMICPP_DETAIL_STRIP_PARENS,									\
		(mock_name, fn_name),											\
		__VA_ARGS__)

/**
 * \brief Starting point for mocking overloaded interface methods.
 * \ingroup MOCK_INTERFACES
 * \param fn_name The overload-set name.
 * \param Others Overloads must be added via MIMICPP_ADD_OVERLOAD macro.
 */
#define MIMICPP_MOCK_OVERLOADED_METHOD(fn_name, ...)						\
	MIMICPP_DETAIL_MAKE_METHOD_OVERRIDES(fn_name##_, fn_name, __VA_ARGS__)	\
	MIMICPP_DETAIL_MAKE_OVERLOADED_MOCK(									\
		fn_name##_,															\
		(MIMICPP_DETAIL_MAKE_SIGNATURE_LIST(__VA_ARGS__)))

/**
 * \brief Starting point for mocking a single interface method.
 * \ingroup MOCK_INTERFACES
 * \param fn_name The overload-set name.
 * \param param_type_list The parameter types.
 * \param Specs An optional parameter for categories (e.g. ``const``, ``noexcept``, etc.).
 */
#define MIMICPP_MOCK_METHOD(fn_name, ret, param_type_list, ...)					\
	MIMICPP_MOCK_OVERLOADED_METHOD(												\
		fn_name,																\
		MIMICPP_ADD_OVERLOAD(ret, param_type_list __VA_OPT__(,) __VA_ARGS__ ))

#endif