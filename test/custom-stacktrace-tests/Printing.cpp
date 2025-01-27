//          Copyright Dominic (DNKpp) Koepke 2024 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "CustomStacktrace.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/trompeloeil.hpp>

using namespace mimicpp;

TEST_CASE(
    "Stacktrace is printable.",
    "[stacktrace]")
{
    using trompeloeil::_;
    auto&& [stacktrace, inner] = std::invoke(
        [] {
            const std::shared_ptr ptr = std::make_shared<CustomBackend::Inner>();
            return std::tuple{
                Stacktrace{CustomBackend{ptr}},
                ptr};
        });

    SECTION("When stacktrace is empty")
    {
        REQUIRE_CALL(inner->emptyMock, Invoke())
            .RETURN(true);

        const auto text = mimicpp::print(stacktrace);
        REQUIRE_THAT(
            text,
            Catch::Matchers::Equals("empty"));
    }

    SECTION("When stacktrace contains one entry.")
    {
        REQUIRE_CALL(inner->emptyMock, Invoke())
            .RETURN(false);
        REQUIRE_CALL(inner->sizeMock, Invoke())
            .RETURN(1u);
        REQUIRE_CALL(inner->sourceMock, Invoke(0u))
            .RETURN("test.cpp");
        REQUIRE_CALL(inner->lineMock, Invoke(0u))
            .RETURN(1337u);
        REQUIRE_CALL(inner->descriptionMock, Invoke(0u))
            .RETURN("Hello, World!");

        const auto text = mimicpp::print(stacktrace);
        REQUIRE_THAT(
            text,
            Catch::Matchers::Equals("test.cpp [1337], Hello, World!\n"));
    }

    SECTION("When stacktrace contains multiple entries.")
    {
        REQUIRE_CALL(inner->emptyMock, Invoke())
            .RETURN(false);
        REQUIRE_CALL(inner->sizeMock, Invoke())
            .RETURN(2u);
        REQUIRE_CALL(inner->sourceMock, Invoke(0u))
            .RETURN("other-test.cpp");
        REQUIRE_CALL(inner->lineMock, Invoke(0u))
            .RETURN(42u);
        REQUIRE_CALL(inner->descriptionMock, Invoke(0u))
            .RETURN("Hello, mimic++!");
        REQUIRE_CALL(inner->sourceMock, Invoke(1u))
            .RETURN("test.cpp");
        REQUIRE_CALL(inner->lineMock, Invoke(1u))
            .RETURN(1337u);
        REQUIRE_CALL(inner->descriptionMock, Invoke(1u))
            .RETURN("Hello, World!");

        const auto text = mimicpp::print(stacktrace);
        REQUIRE_THAT(
            text,
            Catch::Matchers::Equals(
                "other-test.cpp [42], Hello, mimic++!\n"
                "test.cpp [1337], Hello, World!\n"));
    }
}
