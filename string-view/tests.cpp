#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>

#include "catch.hpp"

using namespace std;

TEST_CASE("string_view")
{
    const char* text = "abc def ghi";
    std::string str = "abc def ghi";

    SECTION("creation")
    {
        std::string_view sv1 = text;
        REQUIRE(sv1 == "abc def ghi"sv);
    }

    SECTION("creation")
    {
        std::string_view sv0 = str;
        REQUIRE(sv0.size() == 11);

        std::string_view sv1 = text;
        REQUIRE(sv1 == "abc def ghi"sv);
        REQUIRE(sv1.size() == 11);

        std::string_view sv2(text, 3);
        REQUIRE(sv2 == "abc"sv);
        REQUIRE(sv2.size() == 3);

        std::string_view sv3(str.data(), 3);
        REQUIRE(sv3 == "abc"sv);
        REQUIRE(sv3.size() == 3);

        string_view sv_empty;
        REQUIRE(sv_empty.data() == nullptr); // difference between string & string_view
    }
}

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("print_all")
{
    vector vec = {1, 2, 3};

    print_all(vec, "vec");
    print_all(vec, "vec"s);
}

TEST_CASE("string_view - non null-terminated string")
{
    char tab[] = {'a', 'b', 'c'};

    std::string_view sv(tab, 3);

    REQUIRE(sv == "abc"sv);
}

TEST_CASE("string_view -> string conversion")
{
    std::string_view sv = "text";

    std::string str1(sv);
    std::string str2 = std::string(sv);
}

string_view get_prefix(string_view text, size_t length)
{
    return {text.data(), length};
}

TEST_CASE("beware - danger")
{
    SECTION("safe")
    {
        const char* text = "abcdef";
        string_view pf1 = get_prefix(text, 3);
        REQUIRE(pf1 == "abc"sv);
    }   

    SECTION("string")
    {
        string_view pf2 = get_prefix(string("abcdef"), 3);
        //REQUIRE(pf2 == "abc"sv);

        string_view dangling_text = "text"s;
        std::cout << dangling_text << "\n";
    }
}
        string_view sv_empty;
        REQUIRE(sv_empty.data() == nullptr); // difference between string & string_view
    }
}

template <typename Container>
void print_all(const Container& container, std::string_view prefix)
{
    cout << prefix << ": [ ";
    for (const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("print_all")
{
    vector vec = {1, 2, 3};

    print_all(vec, "vec");
    print_all(vec, "vec"s);
}

TEST_CASE("string_view - non null-terminated string")
{
    char tab[] = {'a', 'b', 'c'};

    std::string_view sv(tab, 3);

    REQUIRE(sv == "abc"sv);
}

TEST_CASE("string_view -> string conversion")
{
    std::string_view sv = "text";

    std::string str1(sv);
    std::string str2 = std::string(sv);
}

string_view get_prefix(string_view text, size_t length)
{
    return {text.data(), length};
}

TEST_CASE("beware - danger")
{
    SECTION("safe")
    {
        const char* text = "abcdef";
        string_view pf1 = get_prefix(text, 3);
        REQUIRE(pf1 == "abc"sv);
    }   

    SECTION("string")
    {
        string_view pf2 = get_prefix(string("abcdef"), 3);
        //REQUIRE(pf2 == "abc"sv);

        string_view dangling_text = "text"s;
        //std::cout << dangling_text << "\n";
    }
}

TEST_CASE("stack & heap")
{
    std::string str = "text";

    std::string_view sv1 = str;
    std::string_view sv2(sv1.data(), 2);

    std::string str2 = str;

    str.clear();
}