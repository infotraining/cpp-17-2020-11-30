#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <map>

#include "catch.hpp"

using namespace std;

void foo()
{
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    [[maybe_unused]] auto target = std::forward<T>(arg);
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto ax1 = 10; // int
    deduce1(10);

    [[maybe_unused]] auto ax2 = cx; // int
    deduce1(cx);

    [[maybe_unused]] auto ax3 = ref_x; // int
    deduce1(ref_x);

    [[maybe_unused]] auto ax4 = cref_x; // int
    deduce1(cref_x);

    [[maybe_unused]] auto ax5 = tab; // int*
    deduce1(tab);

    [[maybe_unused]] auto ax6 = foo; // void(*)()
    deduce1(foo);
}

TEST_CASE("--")
{
    cout << "\n----------\n\n";
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 20;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    [[maybe_unused]] auto& ax1 = x; // int&
    deduce2(x);

    [[maybe_unused]] auto& ax2 = cx; // const int&
    deduce2(cx);

    [[maybe_unused]] auto& ax3 = ref_x; // int&
    deduce2(ref_x);
    deduce2<int&>(ref_x);

    [[maybe_unused]] auto& ax4 = cref_x; // const int&
    deduce2(cref_x);

    [[maybe_unused]] auto& ax5 = tab; // int(&)[10]
    deduce2(tab);

    [[maybe_unused]] auto& ax6 = foo; // void(&)()
    deduce2(foo);
}

TEST_CASE("**")
{
    cout << "\n----------\n\n";
}

TEST_CASE("Template Argument Deduction - case 3")
{
    int x = 10;

    auto&& a1 = x; // int&
    deduce3(x);

    auto&& a2 = 10; // int&&
    deduce3(10);
}

TEST_CASE("%%")
{
    cout << "\n----------\n\n";
}

struct Gadget
{
    string name;
};

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(const Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
}

// void use(Gadget& g)
// {
//     have_fun(g); // forward
// }

// void use(const Gadget& g)
// {
//     have_fun(g); // forward
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g)); // forward
// }

template <typename TGadget>
void use(TGadget&& g)
{
    puts(__PRETTY_FUNCTION__);
    have_fun(std::forward<TGadget>(g));
}

TEST_CASE("have fun with gadget")
{
    Gadget g {"ipad"};
    const Gadget cg {"const ipad"};

    use(g); // 1
    use(cg); // 2
    use(Gadget {"temp"}); // 3
}

template <typename T1, typename T2>
struct ValuePair
{
    T1 fst;
    T2 snd;

    ValuePair(const T1& f, const T2& s)
        : fst {f}
        , snd {s}
    {
    }
};

// deduction guide
template <typename T1, typename T2>
ValuePair(T1, T2) -> ValuePair<T1, T2>;

ValuePair(const char*, const char*)->ValuePair<std::string, std::string>;

TEST_CASE("CTAD")
{
    ValuePair<int, double> vp1 {1, 3.14};

    ValuePair vp2 {1, 3.14}; // ValuePair<int, double>

    ValuePair vp3 {3.14f, "text"s}; // ValuePair<float, string>

    ValuePair vp4 {42, "text"}; // ValuePair<int, const char*>

    ValuePair vp5("abc", "def");
}

TEST_CASE("CTAD - special case")
{
    vector vec {1, 2, 3}; // vector<int>

    vector data {vec}; // vector<int> - SPECIAL CASE - copy syntax

    vector big_data {vec, vec}; // vector<vector<int>>
}

template <typename T1, typename T2>
struct Aggregate
{
    T1 v1;
    T2 v2;
};

template <typename T1, typename T2>
Aggregate(T1, T2) -> Aggregate<T1, T2>;

TEST_CASE("CTAD + aggregates")
{
    Aggregate agg {1, 3.14}; // Aggregate<int, double>
}

TEST_CASE("CTAD + std library")
{
    SECTION("std::pair")
    {
        auto p1 = make_pair(1, 3.14);
        pair<int, string> p2(1, "text");

        const int x = 42;
        auto p3 = pair(x, "text"); // pair<int, const char*>
    }

    SECTION("std::tuple")
    {
        tuple t1 {1, 3.14, "text"}; // tuple<int, double, const char*>

        tuple t2 {t1}; // tuple<int, double, const char*>

        tuple t3 {t1, t2, tuple {"text"s, 4.14}};

        tuple t4 = pair(1, 3.24); // tuple<int, double>
    }

    SECTION("std::optional")
    {
        std::optional o1 = 42; // optional<int>

        std::optional o2 = o1; // optional<int>
    }

    SECTION("smart pointer")
    {
        unique_ptr<int> uptr(new int(32));
        shared_ptr<int> sptr(new int(32));

        shared_ptr sptr2 = std::move(uptr);
        weak_ptr wptr = sptr2;
    }

    SECTION("std::function")
    {
        std::function f = foo;
        f();

        std::function l = [](int x) { return 42; };
    }

    SECTION("std containers")
    {
        vector vec = {1, 2, 3, 4};
        list lst = {4, 5, 6};

        set numbers(lst.begin(), lst.end());

        map mp1 = { pair{1, "one"s}, {2, "two"s} };


        array arr1 = {1, 2, 3, 4}; // array<int, 4>
    }
}

namespace Explain
{
    template <typename T, size_t N>
    struct Array
    {
        T items[N];

        constexpr size_t size() const
        {
            return N;
        }
    };

    // template <typename Head, typename... Tail>
    // Array(Head, Tail...) -> Array<Head, sizeof...(Tail) + 1>;

    template <typename Head, typename... Tail>
    Array(Head, Tail...)
        -> Array<enable_if_t<(is_same_v<Head, Tail> && ...), Head>, sizeof...(Tail) + 1>;
}

TEST_CASE("Array + deduction")
{
    Explain::Array arr = {1, 2, 3};
}