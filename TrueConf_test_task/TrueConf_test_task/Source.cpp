#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <vector>

const size_t ELEMENTS_COUNT = 20;
const size_t MINIMUM = 1;
const size_t MAXIMUM = 9;
const size_t MAX_COUNT_TO_DELETE = 15;

int GenerateRandomNumber(int minimum = MINIMUM, int maximum = MAXIMUM) {
    static std::default_random_engine re(20200620);
    std::uniform_int_distribution<int> random_number(minimum, maximum);
    return random_number(re);
}

std::vector<int> GenerateRandomVector(size_t elements_count = ELEMENTS_COUNT) {
    std::vector<int> random_vector(elements_count);
    for (auto& element : random_vector) {
        element = GenerateRandomNumber();
    }
    return random_vector;
}

std::map<int, int> GenerateRandomMap(size_t elements_count = ELEMENTS_COUNT) {
    std::map<int, int> random_map;
    for (size_t i = 0; i < elements_count; ++i) {
        random_map[i] = GenerateRandomNumber();
    }
    return random_map;
}

std::set<size_t> GenerateIndexesToDelete(size_t collection_size) {
    size_t count_elements_to_delete = std::min(collection_size,
        static_cast<size_t>(GenerateRandomNumber(MINIMUM, MAX_COUNT_TO_DELETE)));

    std::set<size_t> indexes_to_delete;
    while (indexes_to_delete.size() < count_elements_to_delete) {
        indexes_to_delete.insert(GenerateRandomNumber(0, collection_size - 1));
    }

    return indexes_to_delete;
}

void RemoveRandomElementsFromVector(std::vector<int>& random_vector) {
    auto indexes_to_delete = GenerateIndexesToDelete(random_vector.size());

    size_t i = 0;
    size_t j = 0;
    while (i + indexes_to_delete.size() < random_vector.size() && j < random_vector.size()) {
        if (indexes_to_delete.count(j)) {
            j++;
            continue;
        }
        if (random_vector[i] = random_vector[j]) {
            i++;
            j++;
        }
    }

    random_vector.erase(random_vector.begin() + i, random_vector.end());
}

void RemoveRandomElementsFromMap(std::map<int, int>& random_map) {
    auto indexes_to_delete = GenerateIndexesToDelete(random_map.size());

    std::vector <std::map<int, int>::iterator> iterators_to_delete;
    size_t prev_index = 0;
    auto it = random_map.begin();
    for (const auto& index : indexes_to_delete) {
        size_t offset = index - prev_index;
        prev_index = index;
        std::advance(it, offset);
        iterators_to_delete.push_back(it);
    }

    for (auto& it : iterators_to_delete) {
        random_map.erase(it);
    }
}

std::array <bool, MAXIMUM + 1> GetElementsExistingMask(const std::map<int, int>& collection) {
    std::array <bool, MAXIMUM + 1> elements_existing_mask;
    elements_existing_mask.fill(false);
    for (const auto& [key, value] : collection) {
        elements_existing_mask[value] = true;
    }
    return elements_existing_mask;
}

std::array <bool, MAXIMUM + 1> GetElementsExistingMask(const std::vector<int>& collection) {
    std::array <bool, MAXIMUM + 1> elements_existing_mask;
    elements_existing_mask.fill(false);
    for (const auto& element : collection) {
        elements_existing_mask[element] = true;
    }
    return elements_existing_mask;
}

void Synchronyze(std::vector<int>& random_vector, std::map<int, int>& random_map) {
    auto vector_mask = GetElementsExistingMask(random_vector);
    auto map_mask = GetElementsExistingMask(random_map);

    const auto IsInOnlyOneCollections = [&vector_mask, &map_mask](const auto& element) {
        return (vector_mask[element] ^ map_mask[element]);
    };

    auto new_end = remove_if(random_vector.begin(), random_vector.end(), IsInOnlyOneCollections);
    random_vector.erase(new_end, random_vector.end());

    for (auto it = random_map.begin(); it != random_map.end();) {
        if (IsInOnlyOneCollections(it->second)) {
            it = random_map.erase(it);
        } else {
            ++it;
        }
    }
}

void TestGenerateRandomNumber() {
    assert(GenerateRandomNumber(0, 0) == 0);
    std::set<int> numbers = { 1, 2, 3 };
    assert(numbers.count(GenerateRandomNumber(1, 3)));
    assert(numbers.count(GenerateRandomNumber(-10, 0)) == 0);
}

void TestGenerateRandomVector() {
    assert(GenerateRandomVector(100).size() == 100);
    assert(GenerateRandomVector(0).empty());
    auto random_vector = GenerateRandomVector();
    assert(random_vector.size() == ELEMENTS_COUNT);
    for (const auto& element : random_vector) {
        assert(MINIMUM <= element && element <= MAXIMUM);
    }
}

void TestGenerateRandomMap() {
    assert(GenerateRandomMap(100).size() == 100);
    assert(GenerateRandomMap(0).empty());
    auto random_map = GenerateRandomMap();
    assert(random_map.size() == ELEMENTS_COUNT);
    for (const auto& [key, value] : random_map) {
        assert(MINIMUM <= value && value <= MAXIMUM);
    }
}

void TestGenerateIndexesToDelete() {
    auto indexes_to_delete = GenerateIndexesToDelete(ELEMENTS_COUNT);
    assert(*indexes_to_delete.begin() >= 0);
    assert(*(--indexes_to_delete.end()) < ELEMENTS_COUNT);
}

void TestRemoveRandomElements() {
    auto random_vector = GenerateRandomVector();
    size_t vector_size_before_remove = random_vector.size();
    RemoveRandomElementsFromVector(random_vector);
    assert(vector_size_before_remove > random_vector.size());

    auto random_map = GenerateRandomMap();
    size_t map_size_before_remove = random_map.size();
    RemoveRandomElementsFromMap(random_map);
    assert(map_size_before_remove > random_map.size());
}

void TestGetElementsExistingMask() {
    std::array<bool, MAXIMUM + 1> mask;
    mask.fill(false);
    assert(mask == GetElementsExistingMask(std::vector<int>()));
    assert(mask == GetElementsExistingMask(std::map<int, int>()));
    std::vector<int> random_vector = { 0, MAXIMUM };
    std::map<int, int> random_map = { {0, 0}, {1, MAXIMUM} };
    mask[0] = true;
    mask[MAXIMUM] = true;
    assert(mask == GetElementsExistingMask(random_vector));
    assert(mask == GetElementsExistingMask(random_map));
    random_vector.push_back(7);
    random_map[7] = 7;
    assert(mask != GetElementsExistingMask(random_vector));
    assert(mask != GetElementsExistingMask(random_map));
}

void TestSynchronyze() {
    for (size_t i = 0; i < 100; ++i) {
        size_t elements_count = GenerateRandomNumber(0, ELEMENTS_COUNT);
        auto random_vector = GenerateRandomVector(elements_count);
        auto random_map = GenerateRandomMap(elements_count);
        RemoveRandomElementsFromVector(random_vector);
        RemoveRandomElementsFromMap(random_map);
        Synchronyze(random_vector, random_map);
        assert(GetElementsExistingMask(random_vector) == GetElementsExistingMask(random_map));
    }
}

void RunTests() {
    TestGenerateRandomNumber();
    TestGenerateRandomVector();
    TestGenerateRandomMap;
    TestGenerateIndexesToDelete();
    TestRemoveRandomElements();
    TestGetElementsExistingMask();
    TestSynchronyze();
    for (size_t i = 0; i < 10050; ++i) {
        auto random_vector = GenerateRandomVector();
        auto random_map = GenerateRandomMap();
        RemoveRandomElementsFromVector(random_vector);
        RemoveRandomElementsFromMap(random_map);
        Synchronyze(random_vector, random_map);
        assert(GetElementsExistingMask(random_vector) == GetElementsExistingMask(random_map));
    }
    std::cout << "All tests passed\n";
}

int main() {
    RunTests();
    auto random_vector = GenerateRandomVector();
    auto random_map = GenerateRandomMap();
    RemoveRandomElementsFromVector(random_vector);
    RemoveRandomElementsFromMap(random_map);
    Synchronyze(random_vector, random_map);
    return 0;
}