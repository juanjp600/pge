#include "Util.h"

#include <PGE/Types/CircularArray.h>
#include <PGE/Types/Range.h>

using namespace PGE;

TEST_SUITE("Circular Array") {
    
TEST_CASE("Test") {
    CHECK(false);
}

TEST_CASE("Static asserts") {
    using CArray = CircularArray<int>;
    static_assert(std::ranges::random_access_range<CArray>);
    static_assert(std::ranges::sized_range<CArray>);
    static_assert(std::ranges::common_range<CArray>);
}

TEST_CASE("Fill") {
    CircularArray<int> ints(50, 5);
    CHECK(ints.size() == 50);
    for (int i : Range(50)) {
        if (i % 2 == 0) {
            CHECK(ints.back() == 5);
            ints.popBack();
        } else {
            CHECK(ints.front() == 5);
            ints.popFront();
        }
    }
    CHECK(ints.empty());
}

TEST_CASE("Fill with range") {
    std::vector<int> intVec{ 2, 3, 5, 7, 11, 13, 17 };
    CircularArray<int> ints(intVec);
    CHECK(ints.size() == intVec.size());
    for (size_t i : Range(intVec.size())) {
        CHECK(intVec[i] == ints[i]);
    }

    CircularArray<int> ints2{ 1, 2, 3, 4, 5 };
    CHECK(ints2.size() == 5);
    CHECK(ints2.front() == 1);
    CHECK(ints2.back() == 5);
    ints2 = { 0, 1, 1, 2, 3, 5, 8 };
    CHECK(ints2.size() == 7);
    CHECK(ints2.front() == 0);
    CHECK(ints2.back() == 8);

    ints = ints2;
    CHECK(ints.size() == ints2.size());
    CHECK(ints.front() == 0);
    CHECK(ints.back() == 8);

    CircularArray<int> ints3(ints2);
    CHECK(ints3.size() == ints2.size());
    CHECK(ints3.front() == 0);
    CHECK(ints3.back() == 8);

    CircularArray<int> ints4;
    ints4 = std::move(ints3);
    CHECK(ints4.size() == ints2.size());
    CHECK(ints4.front() == 0);
    CHECK(ints4.back() == 8);
    CHECK(ints3.empty());
}

TEST_CASE("Comparisons") {
    CircularArray<int> ints;
    for (int i : Range(5)) {
        ints.pushFront(i * i);
    }
    CHECK((std::vector<int>{ 16, 9, 4, 1, 0 } == ints));
    CHECK((std::vector<int>{ } != ints));
    CHECK((std::vector<int>{ 16, 9, 4, 1 } < ints));
    CHECK((std::vector<int>{ 16, 9, 4, 1, 0, -42 } > ints));
    CHECK((std::vector<int>{ 15 } < ints));
    CHECK((std::vector<int>{ 17 } > ints));
}

using EmplaceFunc = void(CircularArray<int>::*)(const int&);
using PopFunc = void(CircularArray<int>::*)();

using TemplateFunc = std::function<void(CircularArray<int>&, EmplaceFunc, PopFunc)>;

static void testAllPopPushPermutations(const TemplateFunc& func) {
    CircularArray<int> ints;
    func(ints, &CircularArray<int>::pushBack, &CircularArray<int>::popFront);
    ints.clear();
    func(ints, &CircularArray<int>::pushFront, &CircularArray<int>::popBack);
    ints.clear();
    func(ints, &CircularArray<int>::pushBack, &CircularArray<int>::popBack);
    ints.clear();
    func(ints, &CircularArray<int>::pushFront, &CircularArray<int>::popFront);
}

TEST_CASE("Add remove 1") {
    testAllPopPushPermutations([](CircularArray<int>& ints, EmplaceFunc emp, PopFunc pop) {
        for (int i : Range(100)) {
            (ints.*emp)(i);
            (ints.*pop)();
        }
        CHECK(ints.empty());
    });
}

TEST_CASE("Add remove 10") {
    testAllPopPushPermutations([](CircularArray<int>& ints, EmplaceFunc emp, PopFunc pop) {
        for (int i : Range(10)) {
            for (int j : Range(10)) {
                (ints.*emp)(i * 10 + j);
            }
            for (int j : Range(10)) {
                (ints.*pop)();
            }
        }
        CHECK(ints.empty());
    });
}

TEST_CASE("Add back front 1") {
    testAllPopPushPermutations([](CircularArray<int>& ints, EmplaceFunc emp, PopFunc pop) {
        (ints.*emp)(1);
        CHECK(ints.front() == 1);
        CHECK(ints.back() == 1);
    });
}

TEST_CASE("Add back front 1") {
    testAllPopPushPermutations([](CircularArray<int>& ints, EmplaceFunc emp, PopFunc pop) {
        (ints.*emp)(1);
        CHECK(ints.front() == 1);
        CHECK(ints.back() == 1);
    });
}

TEST_CASE("Add back front 2") {
    CircularArray<int> ints;
    ints.pushBack(1);
    ints.pushBack(2);
    CHECK(ints.front() == 1);
    CHECK(ints.back() == 2);

    ints.clear();
    ints.pushFront(2);
    ints.pushFront(1);
    CHECK(ints.front() == 1);
    CHECK(ints.back() == 2);

    ints.clear();
    ints.pushFront(1);
    ints.pushBack(2);
    CHECK(ints.front() == 1);
    CHECK(ints.back() == 2);

    ints.clear();
    ints.pushBack(2);
    ints.pushFront(1);
    CHECK(ints.front() == 1);
    CHECK(ints.back() == 2);
}

TEST_CASE("Add back front 10") {
    CircularArray<int> ints;
    for (int i : Range(10)) {
        ints.pushBack(i);
    }
    CHECK(ints.front() == 0);
    CHECK(ints.back() == 9);

    ints.clear();
    for (int i : Range(10)) {
        ints.pushFront(i);
    }
    CHECK(ints.front() == 9);
    CHECK(ints.back() == 0);
}

TEST_CASE("Empty") {
    CircularArray<int> ints;
    CHECK(ints.empty());
    ints.pushBack(5);
    CHECK_FALSE(ints.empty());
    for (int i : Range(3)) {
        ints.pushFront(5);
    }
    CHECK_FALSE(ints.empty());
}

TEST_CASE("Iterators") {
    CircularArray<int> ints;
    for (int i : ints) {
        CHECK(false);
    }
    for (int i : std::ranges::reverse_view(ints)) {
        CHECK(false);
    }
    for (auto it = ints.rbegin(); it != ints.rend(); it++) {
        CHECK(false);
    }

    CHECK(ints.begin() == ints.end());
    CHECK_THROWS_AS(ints.end()++, Exception);
    CHECK_THROWS_AS(ints.end()--, Exception);

    ints = { 0, 1, 2, 3 };
    CHECK_THROWS_AS(ints.begin()--, Exception);
    CHECK_THROWS_AS(ints.end()++, Exception);
    for (int i = 0; int e : ints) {
        CHECK(e == i++);
    }

    ints = { 0, 0, 0, 1 };
    ints.popFront();
    ints.popFront();
    ints.pushBack(2);
    ints.pushBack(3);
    CHECK_THROWS_AS(ints.begin()--, Exception);
    CHECK_THROWS_AS(ints.end()++, Exception);
    for (int i = 0; int e : ints) {
        CHECK(e == i++);
    }
    for (int i = 3; int e : std::views::reverse(ints)) {
        CHECK(e == i--);
    }
    int i = 3;
    for (CircularArray<int>::ReverseIterator it = ints.rbegin(); it != ints.rend(); it++, i--) {
        CHECK(*it == i);
    }

    const CircularArray<int> constInts{ 1 };
    constInts.begin();
    constInts.end();
    constInts.rbegin();
    constInts.rend();
    constInts[0];
}

TEST_CASE("Iterator spaceship") {
    CircularArray<int> a;
    CHECK(a.begin() == a.begin());
    CHECK(a.end() == a.end());

    CircularArray<int> b;
    CHECK((a.begin() <=> b.begin()) == std::partial_ordering::unordered);
    CHECK(a.begin() != b.begin());
    a = { 1, 2 };
    CHECK(a.begin() < a.end());
    CHECK(a.begin() <= a.begin());
    CHECK(a.end() >= a.begin());
    CHECK(a.end() > a.begin());
}

TEST_CASE("Iterator distance") {
    CircularArray<int> ints;
    CHECK(ints.begin().getPosition() == 0);
    CHECK(ints.end().getPosition() == 0);
    CHECK(ints.end() - ints.begin() == 0);
    for (int i : Range(10)) {
        CHECK(ints.begin().getPosition() == 0);
        CHECK(ints.end().getPosition() == i);
        CHECK(ints.end() - ints.begin() == i);
        CHECK(ints.begin() - ints.end() == -i);
        ints.pushBack(i);
    }
    int i = 0;
    for (CircularArray<int>::Iterator it = ints.begin(); it != ints.end(); it++, i++) {
        CHECK(it.getPosition() == i);
        CHECK(ints.begin() - it == -i);
        CHECK(it - ints.begin() == i);
        CHECK(ints.end() - it == ints.size() - i);
        CHECK(it - ints.end() == i - ints.size());
    }
}

TEST_CASE("Iterator random access") {
    CircularArray<int> ints{ 0, 0, 0, 1 };
    ints.popFront(); ints.popFront();
    ints.pushBack(2); ints.pushBack(3);
    for (int i : Range(4)) {
        for (int j : Range(4)) {
            if (i + j >= 4) {
                CHECK_THROWS_AS((ints.begin() + i)[j], Exception);
            } else {
                CHECK((ints.begin() + i)[j] == i + j);
            }
        }
    }
}

struct MemLeakTester {
    static int counter;
    MemLeakTester() { counter++; }
    MemLeakTester(const MemLeakTester&) { counter++; }
    MemLeakTester(MemLeakTester&&) noexcept { counter++; }
    MemLeakTester& operator=(const MemLeakTester&) = default;
    MemLeakTester& operator=(MemLeakTester&&) noexcept = default;
    ~MemLeakTester() { counter--; }
};

int MemLeakTester::counter = 0;

TEST_CASE("MemLeakTester itself") {
    CHECK(MemLeakTester::counter == 0);
    {
        MemLeakTester a;
        MemLeakTester b(a);
        MemLeakTester c(std::move(a));
    }
    CHECK(MemLeakTester::counter == 0);
}

TEST_CASE("MemLeakTester emplace") {
    CHECK(MemLeakTester::counter == 0);
    {
        CircularArray<MemLeakTester> test;
        for (int i : Range(100)) {
            test.emplaceBack();
        }
    }
    CHECK(MemLeakTester::counter == 0);
}

TEST_CASE("MemLeakTester clear") {
    CHECK(MemLeakTester::counter == 0);
    {
        CircularArray<MemLeakTester> test;
        for (int i : Range(100)) {
            test.emplaceBack();
        }
        test.clear();
    }
    CHECK(MemLeakTester::counter == 0);
}

TEST_CASE("MemLeakTester fill") {
    CHECK(MemLeakTester::counter == 0);
    {
        CircularArray<MemLeakTester> test(25);
        CircularArray<MemLeakTester> test2(25);
        for (int i : Range(25)) {
            if (i % 2 == 0) {
                test2.popBack();
            } else {
                test2.popFront();
            }
        }
    }
    CHECK(MemLeakTester::counter == 0);
}

}
