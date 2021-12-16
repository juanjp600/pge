#include "Util.h"

#include <PGE/Types/CircularArray.h>
#include <PGE/Types/Range.h>

using namespace PGE;

TEST_SUITE("Circular Array") {

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
