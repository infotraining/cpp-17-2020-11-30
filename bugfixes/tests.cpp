#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include "catch.hpp"

using namespace std;

int foo()
{
    static int x;

    return ++x;
}

TEST_CASE("expression evaluation order")
{
    std::cout << foo() << foo() << foo() << "\n";

    std::map<int, int> dict;
    dict[0] = dict.size();

    // for (const auto& [key, value] : dict)
    // {
    //     std::cout << key << " - " << value << "\n";
    // }
}

enum class Coffee : uint8_t
{
    espresso, chemex, v60
};

TEST_CASE("init enums")
{
    Coffee cf{2};

    REQUIRE(cf == Coffee::v60);
}

namespace Explain
{
    enum class byte : uint8_t
    {
    };

    template <class IntegerType>
    constexpr std::byte& operator<<=(std::byte& b, IntegerType shift) noexcept
    {
        return b <<= shift;
    }
}

TEST_CASE("bytes in C++")
{
    uint8_t b1 = 0b001101;

    b1 += 2;

    std::cout << "b1: " << b1 << "\n";

    std::byte b2{0b00110011};
    std::byte b3{0b11010010};

    std::byte result = b2 & b3;
    result |= (b2 << 2);

    std::cout << std::to_integer<uint32_t>(result) << "\n";
}

TEST_CASE("bitset")
{
    std::bitset<32> bs1 = 665;

    std::cout << "bs1: " << bs1 << " - " << bs1.to_ulong() << "\n";
}

TEST_CASE("auto + {}")
{
    int a1 = 10; // init by copy syntax
    int a2{10};  // direct initialization syntax

    auto b1 = 10; // int
    auto b2{10};  // C++17 - int
    auto b3 = {10}; // std::initializer_list<int> 
}

void foo1()
{    
}

void foo2() noexcept
{    
}

template <typename F1, typename F2>
void call(F1 fa, F2 fb)
{
    fa();
    fb();
}

struct Base
{
    virtual ~Base() = default;
    
    virtual void foo() noexcept
    {
        puts(__PRETTY_FUNCTION__);
    }
};

struct Dervied : Base
{
    void foo() noexcept override
    {
        puts(__PRETTY_FUNCTION__);
    }
};

TEST_CASE("noexcept")
{   
    static_assert(!is_same_v<decltype(foo1), decltype(foo2)>);

    void (*fptr)() noexcept = nullptr;

    fptr = foo2;
    //fptr = foo1; // Since C++17 - ERROR

    call(foo1, foo2);
}

std::vector<int> create_vector_rvo(size_t n)
{
    return std::vector<int>(n, -1);
}

std::vector<int> create_vector_nrvo(size_t n)
{
    vector<int> vec(n);
    std::iota(begin(vec), end(vec), 0);
    
    return vec; // lvalue
}

TEST_CASE("copy ellision & RVO")
{
    vector<int> vec1 = create_vector_rvo(1'000'000);
    REQUIRE(vec1.size() == 1'000'000);

    vector<int> vec2 = create_vector_nrvo(1'000'000); // move constructor may be called
    REQUIRE(vec2.size() == 1'000'000);
}

struct CopyMoveDisabled
{
    vector<int> data;

    CopyMoveDisabled(std::initializer_list<int> lst)
        : data {lst}
    {
    }

    CopyMoveDisabled(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled& operator=(const CopyMoveDisabled&) = delete;
    CopyMoveDisabled(CopyMoveDisabled&&) = delete;
    CopyMoveDisabled& operator=(CopyMoveDisabled&&) = delete;
    ~CopyMoveDisabled() = default;
};

CopyMoveDisabled create_impossible()
{
    return CopyMoveDisabled{1, 2, 3};
}

void use(CopyMoveDisabled arg)
{
    std::cout << "Using object: ";
    for(const auto& item : arg.data)
        std::cout << item << " ";
    std::cout << "\n";
}

TEST_CASE("CopyMoveDisabled")
{
    CopyMoveDisabled cmd = create_impossible();
    cmd.data.clear();

    use(CopyMoveDisabled{6, 7, 8});
    use(create_impossible());

    CopyMoveDisabled&& ref = create_impossible();
    ref.data.clear();
    create_impossible().data.size(); // materialization
}

std::atomic<int> create_value(int value)
{
    return std::atomic<int>(value); // since C++17 - it is possible
}