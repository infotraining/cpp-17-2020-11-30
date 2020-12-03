#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <atomic>
#include <charconv>
#include <string_view>
#include <array>

#include "catch.hpp"

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> o1 = 42;
    REQUIRE(o1.has_value() == true);

    std::optional<int> o2;
    REQUIRE(o2.has_value() == false);

    std::optional<int> o3 = std::nullopt;
    REQUIRE(!o3);

    // CTAD
    std::optional text = "abc"s; // std::optional<std::string>

    std::optional<std::atomic<int>> counter(std::in_place, 42);

    if (counter)
    {
        std::cout << *counter << "\n";
    }

    SECTION("access to value")
    {
        optional val = 42;

        SECTION("unsafe")
        {            
            REQUIRE(*val == 42);  // UB
        }

        SECTION("safe")
        {
            REQUIRE(val.value() == 42);
            
            val.reset();
            val = std::nullopt;
            
            REQUIRE_THROWS_AS(val.value(), std::bad_optional_access);
            
            optional<int> empty;
            REQUIRE_THROWS_AS(empty.value(), std::bad_optional_access);

            SECTION("value_or()")
            {
                optional name = "Jan";

                REQUIRE(name.value_or("(not-set)") == "Jan"s);

                name = std::nullopt;

                REQUIRE(name.value_or("(not-set)") == "(not-set)"s);
            }
        }
    }
}

std::optional<const char*> maybe_getenv(const char* n)
{
    if (const char* x = std::getenv(n))
        return x;
    else
        return std::nullopt;
}

TEST_CASE("maybe_getenv")
{
    optional path = maybe_getenv("PATHs");

    std::cout << path.value_or("PATH not found") << "\n";
}

TEST_CASE("optional & move semantics")
{
    optional data = vector{1, 2, 3};

    optional target = std::move(data);    

    REQUIRE(data.has_value() == true);
    REQUIRE(target.has_value());
    REQUIRE(*target == vector{1, 2, 3});
}

TEST_CASE("strange cases")
{
    SECTION("optional<bool>")
    {
        optional<bool> ob{false};

        if (ob)
        {
            std::cout << "ob has value..." << *ob << "\n";
        }

        REQUIRE(ob == false);
    }

    SECTION("optional<int*>")
    {
        std::optional<int*> optr{nullptr};

        if (optr)
        {
            std::cout << "optr has value..." << *optr << "\n";
        }

        REQUIRE(optr == nullptr);
    }
}

TEST_CASE("optional<const int>")
{
    std::optional<const int> oc1;
    REQUIRE(oc1.has_value() == false);

    std::optional<const int> oc2 = 42;
    REQUIRE(oc2.has_value());

    REQUIRE(*oc2 == 42);
    auto& value = *oc2;
    //++value;

    oc2.reset();

    oc2.emplace(665);
    REQUIRE(*oc2 == 665);
}

namespace Explain
{
    template <typename T>
    class Optional
    {
        alignas(T) char object_[sizeof(T)];        
        bool has_state_;
    };
}

TEST_CASE("optional + const")
{
    std::optional<int> o = 42;
    *o = 665;
    o.reset();

    std::optional<const int> o2c = 42;
    // *o = 665; // ERROR
    o2c.reset();

    const std::optional<int> co = 42;
    // co.value() = 665; // ERROR
    // co.reset(); // ERROR

    const std::optional<const int> co2c = 42;
    // *co2c = 665; // ERROR
    //co2c.reset(); // ERROR
}

std::optional<int> to_int(const string_view str)
{
    int value{};
    
    auto start = str.data();
    auto end = str.data() + str.size();

    if (const auto [pos_end, error_code] = std::from_chars(start, end, value); 
        error_code != std::errc{} || pos_end != end)
    {
        return nullopt;
    }

    return value;
}

TEST_CASE("to_int")
{
    optional<int> number = to_int("42");
    REQUIRE(number.has_value());
    REQUIRE(*number == 42);

    optional age = to_int("abc"s);
    REQUIRE(age.has_value() == false);
}

template <typename TContainer>
constexpr std::optional<std::string_view> find_id(const TContainer& container, std::string_view id)
{
    for(const auto& item : container)
        if (item == id)
            return item;
    
    return nullopt;
}

TEST_CASE("string_view + optional + constexr")
{
    constexpr string_view id = "Ericsson";
    static_assert(id.size() == 8);

    constexpr std::array ids = { "one"sv, "two"sv, "three"sv };

    static_assert(ids[1] == "two"sv);

    constexpr optional opt_id = find_id(ids, "two"sv);
    static_assert(opt_id.has_value());
}