#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <TVector.h>
#include <cstdio>

using namespace MindShake;

//-------------------------------------
class Fixture {
    public:
        TVector<int>        ints1;
        TVector<int>        ints2 { 0, 1, 2 };
        std::vector<int>    ints3 { -1, -2 };
};

//-------------------------------------
struct KK {
    KK()                { ++count; }
    KK(const KK &)      { ++count; }
    KK(KK &&) noexcept  { ++count; }
    ~KK()               { --count; }

    static void reset() { count = 0; }

    static int count;
};

int KK::count = 0;

//-------------------------------------
int funcTVector(TVector<int> &v) {
    int sum = 0;
    for (auto &i : v) {
        i *= 2;
        sum += i;
    }
    return sum;
}

//-------------------------------------
int funcStdVector(std::vector<int> &v) {
    int sum = 0;
    for (auto &i : v) {
        i *= 2;
        sum += i;
    }
    return sum;
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Constructors / operator =") {
    TVector<int>    aux1(ints1);
    TVector<int>    aux2(ints2);
    TVector<int>    aux3(ints3);
    TVector<int>    aux4({1, 2, 3});

    CHECK(aux1 == ints1);
    CHECK(aux2 == ints2);
    CHECK(aux3 == ints3);
    CHECK(aux4 == std::vector<int>{1, 2, 3});

    CHECK(aux1 != ints2);
    CHECK(aux2 != ints3);
    CHECK(aux3 != ints1);

    aux1 = ints2;
    CHECK(aux1 != ints1);
    CHECK(aux1 == ints2);

    aux1 = ints3;
    CHECK(aux1 != ints2);
    CHECK(aux1 == ints3);

    {
        TVector<KK> kk1 = { {}, {} };
        CHECK(KK::count == 2);

        auto *kk2 = new TVector<KK>(kk1);
        CHECK(KK::count == 4);
        delete kk2;
    }
    CHECK(KK::count == 0);
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Element access") {
    CHECK(ints2[0] == 0);
    CHECK(ints2.at(0) == 0);
    CHECK_THROWS_AS(ints2.at(10), std::out_of_range);
    CHECK_THROWS_AS(ints2.at(-10), std::out_of_range);
    CHECK(ints2.front() == 0);
    CHECK(ints2.back() == 2);
    CHECK(*ints2.begin() == 0);
    CHECK(*ints2.begin() == 0);
    CHECK(*ints2.rbegin() == 2);
    CHECK(*ints2.crbegin() == 2);

    auto *data = ints2.data();
    // Forward [begin - end)
    size_t index = 0;
    for (auto &v : ints2) {
        CHECK(v == ints2[index]);
        CHECK(v == ints2.at(index));
        CHECK(v == data[index]);
        ++index;
    }

    // Forward const [begin - end)
    index = 0;
    for (const auto &v : ints2) {
        CHECK(v == ints2[index]);
        CHECK(v == ints2.at(index));
        CHECK(v == data[index]);
        ++index;
    }

    // Backward [rbegin - rend)
    index = ints2.size() - 1;
    for (auto it = ints2.rbegin(); it != ints2.rend(); ++it) {
        CHECK(*it == ints2[index]);
        CHECK(*it == ints2.at(index));
        CHECK(*it == data[index]);
        --index;
    }

    // Backward const [rbegin - rend)
    index = ints2.size() - 1;
    for (auto it = ints2.crbegin(); it != ints2.crend(); ++it) {
        CHECK(*it == ints2[index]);
        CHECK(*it == ints2.at(index));
        CHECK(*it == data[index]);
        --index;
    }

    TVector<int> ints { 1, 2, 3 };
    ints[-1] = -1;
    ints[-2] = -2;
    ints[-3] = -3;
    CHECK(ints == TVector<int> { -3, -2, -1 });
    ints.at(-1) = 3;
    ints.at(-2) = 2;
    ints.at(-3) = 1;
    CHECK(ints == TVector<int> { 1, 2, 3 });
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Capacity") {
    ints1.clear();
    CHECK(ints1.empty());

    ints1.reserve(16);
    CHECK(ints1.capacity() == 16);
    CHECK(ints1.size() == 0);

    ints1 = ints2;
    ints1.shrink_to_fit();
    CHECK(ints1.capacity() == ints1.size());

    ints1.resize(16);
    CHECK(ints1.size() == 16);

    ints1.resize(1);
    CHECK(ints1.size() == 1);
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Modifiers") {
    TVector<int> aux { 1, 2, 3 };

    ints1.clear();
    ints2.clear();

    SUBCASE("Insert") {
        ints1.insert(ints1.begin(), 0);
        ints1.insert(ints1.begin() + 1, aux.begin(), aux.end());
        ints1.insert(ints1.begin() + 4, 2, 4);
        ints1.insert(ints1.begin(), { -3, -2, -1 });
        ints1.insert(ints1.end(), aux.begin(), aux.end());

        ints2.insert(0, 0);
        ints2.insert(1, aux.begin(), aux.end());
        ints2.insert(4, 2, 4);
        ints2.insert(0, { -3, -2, -1 });
        ints2.insert(-1, aux);

        CHECK(ints1 == ints2);

        TVector<int> ints { 1, 2, 3 };
        ints.erase_quick(0); // {3, 2}
        //int a = ints[-1];       // 3
        //ints[-1] = 42;          // {1, 2, 42}
        //
        //int b = ints.at(-2);    // 2
        //ints.at(-2) = -42;      // {1, -42, 42}
        //
        //ints.insert(-1, 123);   // {1, -42, 42, 123}
        //ints.emplace(-2, 456);   // {1, -42, 42, 456, 123}
        //ints.erase(-2);         // {1, -42, 42, 123}
        int a22 = 0;
    }

    SUBCASE("Emplace") {
        ints1.emplace(ints1.begin(), 0);
        ints1.emplace(ints1.begin() + 1, 1);
        ints1.emplace(ints1.end(), 3);
        ints1.emplace(ints1.end() - 1, 2);

        ints2.emplace(0, 0);
        ints2.emplace(1, 1);
        ints2.emplace(-1, 3);
        ints2.emplace(-2, 2);
        CHECK(ints1 == ints2);
    }

    SUBCASE("Erase") {
        TVector<int> ints = { 1, 2, 3, 4, 5, 6 };

        ints.erase(0);
        CHECK(ints.size() == 5);
        ints.erase(ints.size() - 1);
        CHECK(ints.size() == 4);
        ints.erase(ints.size() / 2);
        CHECK(ints.size() == 3);
        ints.erase(1, ints.size());
        CHECK(ints.size() == 1);
        ints.erase(0, -1); // Do not erase anything
        CHECK(ints.empty() == false);
        ints.erase(-1);
        CHECK(ints.empty());

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.erase(2, -2);
        CHECK(ints == TVector<int> { 0, 1, 4, 5 });

        ints = { 0, 1, 2, 3, 4, 5 };

        ints.erase(-1);
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4 });
        ints.erase(-2);
        CHECK(ints == TVector<int> { 0, 1, 2, 4 });
        ints.erase(-3);
        CHECK(ints == TVector<int> { 0, 2, 4 });
        ints.erase(-3);
        CHECK(ints == TVector<int> { 2, 4 });
        ints.erase(-2);
        CHECK(ints == TVector<int> { 4 });
        ints.erase(-1);
        CHECK(ints == TVector<int> { });

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.erase(5);
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4 });
        ints.erase(4);
        CHECK(ints == TVector<int> { 0, 1, 2, 3 });
        ints.erase(3);
        CHECK(ints == TVector<int> { 0, 1, 2 });
        ints.erase(2);
        CHECK(ints == TVector<int> { 0, 1 });
        ints.erase(1);
        CHECK(ints == TVector<int> { 0 });
        ints.erase(0);
        CHECK(ints == TVector<int> { });
    }

    SUBCASE("EraseQuick") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };

        // Reorders the elements
        ints.erase_quick(-1);
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4 });
        ints.erase_quick(-2);
        CHECK(ints == TVector<int> { 0, 1, 2, 4 });
        ints.erase_quick(-3);
        CHECK(ints == TVector<int> { 0, 4, 2 });
        ints.erase_quick(-3);
        CHECK(ints == TVector<int> { 2, 4 });
        ints.erase_quick(-2);
        CHECK(ints == TVector<int> { 4 });
        ints.erase_quick(-1);
        CHECK(ints == TVector<int> { });

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { 5, 1, 2, 3, 4 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { 4, 1, 2, 3 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { 3, 1, 2 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { 2, 1 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { 1 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { });

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.erase_quick(5);
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4 });
        ints.erase_quick(4);
        CHECK(ints == TVector<int> { 0, 1, 2, 3 });
        ints.erase_quick(3);
        CHECK(ints == TVector<int> { 0, 1, 2 });
        ints.erase_quick(2);
        CHECK(ints == TVector<int> { 0, 1 });
        ints.erase_quick(1);
        CHECK(ints == TVector<int> { 0 });
        ints.erase_quick(0);
        CHECK(ints == TVector<int> { });
    }

    SUBCASE("push / pop") {
        ints1.clear();

        ints1.push_back(1);
        CHECK(ints1.size() == 1);
        ints1.pop_back();
        CHECK(ints1.empty());

        ints1.push_back(1);
        ints1.push_back_if_new(1);
        CHECK(ints1.size() == 1);

        ints1.emplace_back(2);
        CHECK(ints1.size() == 2);

        ints1.emplace_back_if_new(2);
        CHECK(ints1.size() == 2);

        ints1 = {1, 2, 3, 4, 5, 6};
        ints1.resize(3);
        CHECK(ints1 == TVector<int>{1, 2, 3});
        ints1.resize(0);
        CHECK(ints1.empty());
        ints1.resize(6, 3);
        CHECK(ints1 == TVector<int>{3, 3, 3, 3, 3, 3});

        ints2 = {1, 2, 3, 4, 5, 6};
        ints2.swap(ints1);
        CHECK(ints1 == std::vector<int> { 1, 2, 3, 4, 5, 6 });
        CHECK(ints2 == TVector<int> { 3, 3, 3, 3, 3, 3 });
    }
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Conversion") {
    ints1 = ints2;
    CHECK(funcTVector(ints1) == funcStdVector(ints2));

    std::vector<int> ints4 = ints3;
    CHECK(funcTVector(asTVector(ints3)) == funcStdVector(ints4));
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Search operations") {
    SUBCASE("Find") {
        for (const auto &i : ints2) {
            CHECK(ints2.find(i) != ints2.end());
        }
        CHECK(ints2.find(10) == ints2.end());

        CHECK(ints2.find_if([](int v) { return (v % 2) == 1; }) != ints2.end());
        CHECK(ints2.find_if_not([](int v) { return v < 5; }) == ints2.end());

        TVector<int> ints = { 0, 1, 2, 1, 0 };
        auto it = ints.find_last(1);
        *it = -*it;
        CHECK(ints == TVector<int> { 0, 1, 2, -1, 0 });

        it = ints.find_last_if([](int v) { return v == 2; });
        *it = -*it;
        CHECK(ints == TVector<int> { 0, 1, -2, -1, 0 });

        it = ints.find_last_if_not([](int v) { return v <= 0; });
        *it = -*it;
        CHECK(ints == TVector<int> { 0, -1, -2, -1, 0 });

        CHECK(ints.find_last(10) == ints.crend());
        CHECK(ints.find_last_if([](int v) { return v == 10;  }) == ints.crend());
        CHECK(ints.find_last_if_not([](int v) { return v <= 0;  }) == ints.crend());

        CHECK(ints.count(0) == 2);
        CHECK(ints.count_if([](int v) { return v < 0; }) == 3);

        CHECK(ints.all_of([](int v) { return v >= -2; }));
        CHECK(ints.any_of([](int v) { return v < 0; }));
        CHECK(ints.none_of([](int v) { return v > 0; }));
    }

    SUBCASE("Contains") {
        for (const auto &i : ints2) {
            CHECK(ints2.contains(i));
        }
        CHECK(ints2.contains(10) == false);
    }

    SUBCASE("Get Index") {
        for (size_t i = 0; i < ints2.size(); ++i) {
            CHECK(ints2.get_index(ints2[i]) == i);
        }
        CHECK(ints2.get_index(10) == -1);
    }
}

//-------------------------------------
TEST_CASE_FIXTURE(Fixture, "Algorithms") {
    SUBCASE("Transform") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };
        TVector<int> aux;
        // aux is resized (must have almost the same number of elements than ints)
        ints.transform(aux, [](const int &i) -> int { return i * 2; });
        CHECK(aux == TVector<int> { 0, 2, 4, 6, 8, 10 });

        aux.resize(aux.size() * 2);
        ints.transform(aux.begin() + ints.size(), [](int i) -> int { return i + 1; });
        CHECK(aux == TVector<int> { 0, 2, 4, 6, 8, 10, 1, 2, 3, 4, 5, 6 });

        TVector<float> floats = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
        TVector<float> result(floats.size());
        // floats and result must have almost the size of ints
        ints.transform(floats.begin(), result.begin(), [](auto a, auto b) -> decltype(a + b) { return a + b; });
        CHECK(result == TVector<float> { 0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f });

        TVector<float> result2;
        //result2.resize(ints.size());
        ints.transform(result2, [](auto i) { return i * 2.0f; });
        CHECK(result2 == TVector<float> { 0.0f, 2.0f, 4.0f, 6.0f, 8.0f, 10.0f });

        // Similar a for_each
        ints.transform(ints, [](const int &i) -> int { return i * 2; });
        CHECK(ints == TVector<int> { 0, 2, 4, 6, 8, 10 });
    }

    SUBCASE("Replace") {
        TVector<int> ints { 0, 1, 2, 3, 4, 5 };
        TVector<int> ints2, ints3;

        ints.replace(2, -2)
            .replace(1, -1);
        CHECK(ints == TVector<int> { 0, -1, -2, 3, 4, 5 });

        ints.replace_if([](int v) { return v >= 3; }, 3)
            .replace_if([](int v) { return v < 3; }, 2);
        CHECK(ints == TVector<int> { 2, 2, 2, 3, 3, 3 });

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.replace_if([](int v) { return v >= 3; },
                        [](int v) { return v * 2;  });
        CHECK(ints == TVector<int> { 0, 1, 2, 6, 8, 10 });

        ints = { 0, 1, 0 };
        ints.replace(0, 2);
        CHECK(ints == TVector<int> { 2, 1, 2 });
    }

    SUBCASE("Replace copy") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };
        TVector<int> output;

        ints.replace_copy(output, 2, -2);
        CHECK(output == TVector<int> { 0, 1, -2, 3, 4, 5 });

        ints.replace_copy_if(output.clear(), [](int v) { return v >= 3; }, 3);
        CHECK(output == TVector<int> { 0, 1, 2, 3, 3, 3 });

        ints = { 0, 1, 2, 3, 4, 5 };
        ints.replace_copy_if(output.clear(), [](int v) { return v >= 3; }, [](int v) { return v * 2; });
        CHECK(output == TVector<int> { 0, 1, 2, 6, 8, 10 });
    }

    SUBCASE("Copy if") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };
        TVector<int> result;

        ints.copy_if(result, [](int v) { return (v & 1) == 0; });
        CHECK(result == TVector<int> { 0, 2, 4 });

        result = ints.filter([](int v) { return (v & 1) == 1; });
        CHECK(result == TVector<int> { 1, 3, 5 });
    }

    SUBCASE("For each") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };

        ints.for_each([](int &i) { i *= 2; })
            .for_each([](int &i) { i *= 2; });
        CHECK(ints == TVector<int> { 0, 4, 8, 12, 16, 20 });
    }

    SUBCASE("Sort") {
        TVector<int> ints = { 3, 2, 1, 0, 4, 5 };
        ints.sort();
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4, 5 });

        ints.sort(std::greater<int>());
        CHECK(ints == TVector<int> { 5, 4, 3, 2, 1, 0 });

        ints.sort([](const int &a, const int &b) { return a < b;  });
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4, 5 });

        //--
        ints = { 3, 2, 1, 0, 4, 5 };
        ints.stable_sort();
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4, 5 });

        ints.stable_sort(std::greater<int>());
        CHECK(ints == TVector<int> { 5, 4, 3, 2, 1, 0 });

        ints.stable_sort([](const int &a, const int &b) { return a < b;  });
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4, 5 });

        //--
        ints = { 3, 3, 1, 1, 4, 4 };
        ints.sort()
            .unique()
            .reverse();
        CHECK(ints == TVector<int> { 4, 3, 1 });

        ints.shuffle(); // We cannot check it
    }

    SUBCASE("Rotate") {
        TVector<int> ints = { 0, 1, 2, 3, 4, 5 };
        ints.rotate(2);
        CHECK(ints == TVector<int> { 2, 3, 4, 5, 0, 1 });
        ints.rotate(-2);
        CHECK(ints == TVector<int> { 0, 1, 2, 3, 4, 5 });
        ints.rotate(-2);
        CHECK(ints == TVector<int> { 4, 5, 0, 1, 2, 3 });
    }

    SUBCASE("Min/Max") {
        TVector<int> ints;

        CHECK(ints.min() == 0);
        CHECK(ints.min(std::greater<int>()) == 0);

        CHECK(ints.max() == 0);
        CHECK(ints.max(std::greater<int>()) == 0);
        {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            const auto &[a, b] = ints.minmax();
            CHECK(((a == 0) && (b == 0)));

            const auto &[c, d] = ints.minmax(std::greater<int>());
            CHECK(((c == 0) && (d == 0)));
#else
            auto p1 = ints.minmax();
            CHECK(((p1.first == 0) && (p1.second == 0)));

            auto p2 = ints.minmax(std::greater<int>());
            CHECK(((p2.first == 0) && (p2.second == 0)));
#endif
        }

        ints = { 0, 1, 2, 3, 4, 5 };
        CHECK(ints.min() == 0);
        CHECK(ints.min(std::greater<int>()) == 5);

        CHECK(ints.max() == 5);
        CHECK(ints.max(std::greater<int>()) == 0);

        {
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
            const auto &[a, b] = ints.minmax();
            CHECK(((a == 0) && (b == 5)));

            const auto &[c, d] = ints.minmax(std::greater<int>());
            CHECK(((c == 5) && (d == 0)));
#else
            auto p1 = ints.minmax();
            CHECK(((p1.first == 0) && (p1.second == 5)));

            auto p2 = ints.minmax(std::greater<int>());
            CHECK(((p2.first == 5) && (p2.second == 0)));
#endif
        }
    }
}