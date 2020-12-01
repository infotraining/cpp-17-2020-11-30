#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

struct Person
{
    inline static int id_gen = 0;

    const int id = ++id_gen;
};

TEST_CASE("inline static")
{
    Person p1;
    Person p2;

    REQUIRE(p1.id == 1);
    REQUIRE(p2.id == 2);
}

template <typename TContainer>
void print(string_view prefix, const TContainer& container)
{
    std::cout << prefix << ": [ ";
    for (const auto& item : container)
        std::cout << item << " ";
    std::cout << "]\n";
}

struct Aggregate1
{
    int a;
    double b;
    int coord[3];
    std::string name;

    void print() const
    {
        std::cout << "Aggregate1{" << a << ", " << b;
        std::cout << ", [ ";
        for (const auto& item : coord)
            std::cout << item << " ";
        std::cout << "], '" << name << "'}\n";
    }
};

TEST_CASE("C++20 - future")
{
    Aggregate1 agg1 {.b = 3.14};
}

TEST_CASE("aggregate")
{
    SECTION("arrays")
    {
        int tab1[5];
        print("tab1", tab1);

        int tab2[5] = {1, 2, 3, 4, 5};
        print("tab2", tab2);

        int tab3[5] = {};
        print("tab3", tab3);

        int tab4[5] = {1, 2, 3};
        print("tab4", tab4);
    }

    Aggregate1 agg1 {42, 3.14, {}, "abc"};
    agg1.print();

    Aggregate1 agg2 {};
    agg2.print();
}

// since C++17
struct Aggregate2 : std::string, Aggregate1
{
    std::vector<int> data;
};

TEST_CASE("aggregates in C++17")
{
    Aggregate2 object {"abc", {42, 3.14, {1, 2}}, {5, 6, 7}};

    REQUIRE(object.size() == 3);
    object.print();
    REQUIRE(object.data == vector {5, 6, 7});

    static_assert(std::is_aggregate_v<Aggregate2>);
}

struct ErrorCode
{
    int err_code;
    const char* msg;
};

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    [[maybe_unused]] int flag = 42;

    return ErrorCode {13, "Error#13"};
}

void step1() { }
void step2() { }
void step3() { }

namespace [[deprecated]] LegacyCode
{
    namespace Lib::Ver1_0
    {
        void f(int n)
        {
            switch (n)
            {
            case 1:
            case 2:
                step1();
                [[fallthrough]];
            case 3: // no warning on fallthrough
                step2();
                break;
            case 4: // compiler may warn on fallthrough
                step3();
            }
        }
    }
}

TEST_CASE("attributes")
{
    auto result = open_file("data.txt");

    REQUIRE(result.err_code == 13);

    LegacyCode::Lib::Ver1_0::f(42);
}