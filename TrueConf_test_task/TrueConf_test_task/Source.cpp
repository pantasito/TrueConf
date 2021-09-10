#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <vector>

const size_t ELEMENTS_COUNT = 10000;
const size_t MINIMUM = 1;
const size_t MAXIMUM = 9;
const size_t MAX_COUNT_TO_DELETE = 15;

int GenerateRandomNumber(int minimum = MINIMUM, int maximum = MAXIMUM) {
    static std::default_random_engine re(20200620);
    std::uniform_int_distribution<int> random_number(minimum, maximum);
    return random_number(re);
}

std::vector<int> GenerateRandomVector() {
    std::vector<int> random_vector(ELEMENTS_COUNT);
    for (auto& element : random_vector) {
        element = GenerateRandomNumber();
    }
    return random_vector;
}

std::map<int, int> GenerateRandomMap() {
    std::map<int, int> random_map;
    for (size_t i = 0; i < ELEMENTS_COUNT; ++i) {
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

std::array <bool, MAXIMUM + 1> GetElementExistingMask(const std::map<int, int>& collection) {
    std::array <bool, MAXIMUM + 1> is_in_collection;
    is_in_collection.fill(false);
    for (const auto& [key, value] : collection) {
        is_in_collection[value] = true;
    }
    return is_in_collection;
}

std::array <bool, MAXIMUM + 1> GetElementExistingMask(const std::vector<int>& collection) {
    std::array <bool, MAXIMUM + 1> is_in_collection;
    is_in_collection.fill(false);
    for (const auto& element : collection) {
        is_in_collection[element] = true;
    }
    return is_in_collection;
}

void Synchronyze(std::vector<int>& random_vector, std::map<int, int>& random_map) {
    auto is_in_random_vector = GetElementExistingMask(random_vector);
    auto is_in_random_map = GetElementExistingMask(random_map);

    const auto IsElementInBothCollections = [&is_in_random_vector, &is_in_random_map](const auto& element) {
        return !is_in_random_vector[element] || !is_in_random_map[element];
    };

    auto new_end = remove_if(random_vector.begin(), random_vector.end(), IsElementInBothCollections);
    random_vector.erase(new_end, random_vector.end());

    for (auto it = random_map.begin(); it != random_map.end();) {
        if (IsElementInBothCollections(it->second)) {
            it = random_map.erase(it);
        }
        else {
            ++it;
        }
    }
}

int main() {
    auto random_vector = GenerateRandomVector();
    auto random_map = GenerateRandomMap();
    RemoveRandomElementsFromVector(random_vector);
    RemoveRandomElementsFromMap(random_map);
    Synchronyze(random_vector, random_map);
    return 0;
}