#include <iostream> 
#include <string> 
#include <vector> 
#include <fstream>
#include <utility> 
#include <sstream> 
#include <stdlib.h>

using Rucksack = std::pair<std::string, std::string>; 

constexpr const int LOWERCASE_SHIFT = 96; // 'a' = 97
constexpr const int UPPERCASE_SHIFT = (65-27); // 'A' = 65

void get_data(const std::string&, std::vector<Rucksack>&); 
void solve_part_one(const std::vector<Rucksack>&); 
void solve_part_two(const std::vector<Rucksack>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 3 <<<" << std::endl; 

    std::string file_name{}; 
    bool is_test{false}; 
    std::vector<Rucksack> rucksacks; 

    if (argc > 1 && std::string{argv[1]} == "test") {
        is_test = true; 
    }

    if (is_test) {
        file_name = "./test.txt"; 
    } else {
        file_name = "./input.txt";
    }

    get_data(file_name, rucksacks); 

    if (is_test) {
        for (const auto& r: rucksacks) {
            std::cout << "compartment A: " << r.first << ", compartment B: " << r.second << std::endl;
        }
    }

    solve_part_one(rucksacks); 
    solve_part_two(rucksacks); 

}

void get_data(const std::string& file_name, std::vector<Rucksack>& rucksacks) {

    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line{}; 
        while (std::getline(input_file, line)) {
            auto n = line.size(); 
            auto compartmentA = line.substr(0, n/2); 
            auto compartmentB = line.substr((n/2), (n - (n/2))); 
            Rucksack rucksack(compartmentA, compartmentB); 
            rucksacks.push_back(rucksack); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
}

// Approach: compare compartment A with compartment B char by char 
// and stop when the common element is found. 
// To speed up, a tmp stream 'unique_chars' keeps track of the visited elements 
auto compare_compartments(const std::string& A, const std::string& B) {
    std::ostringstream unique_chars;
    int priority = 0; 

    for (const auto& c: A) {
        if (unique_chars.str().find(c) == std::string::npos) { 
            if (B.find(c) != std::string::npos) {
                priority = (c - 0 >= 96) ? c - LOWERCASE_SHIFT : c - UPPERCASE_SHIFT; 
                break;
            } 
            unique_chars << c; 
        }
    } 
    return priority; 
}

auto calculate_priorities(const std::vector<Rucksack>& rucksacks) {
    int total_priority = 0; 
    for (const auto& r: rucksacks) {
        auto compartmentA = r.first; 
        auto compartmentB = r.second; 
        total_priority += compare_compartments(compartmentA, compartmentB); 
    }
    return total_priority; 
}

void solve_part_one(const std::vector<Rucksack>& rucksacks) {
    auto priority = calculate_priorities(rucksacks); 
    std::cout << "The solution to part one is: " << priority << std::endl; 
}

// Approach: compare first vs. second and store the common chars in the common_badges stream 
// Then, compare this partial result of 1 vs. 2 common badges with third 
auto compare_group(const std::string& first, const std::string& second, const std::string& third) {
    std::ostringstream unique_chars; 
    std::ostringstream common_badges; 
    int badge_priority = 0; 
    for (const auto& c1: first) {
        if (unique_chars.str().find(c1) == std::string::npos) {
            if (second.find(c1) != std::string::npos) {
                common_badges << c1; 
            }
            unique_chars << c1; 
        }
    }

    // See https://stackoverflow.com/questions/5288036/how-to-clear-ostringstream#:~:text=str(%22%22)%3B%20s.,need%20to%20call%20clear()%20.
    // for why we need both the following commands 
    unique_chars.str(""); unique_chars.clear(); 

    for (const auto& c2: common_badges.str()) {
        if (unique_chars.str().find(c2) == std::string::npos) {
            if (third.find(c2) != std::string::npos) {
                badge_priority = (c2 - 0 >= 96) ? c2 - LOWERCASE_SHIFT : c2 - UPPERCASE_SHIFT;  
                break; 
            }
            unique_chars << c2; 
        }
    }
    return badge_priority; 
}

auto calculate_priorities_by_group(const std::vector<Rucksack>& rucksacks) {
    int total_priority = 0; 
    int group_index = 0; 
    std::string first, second, third; 

    for (const auto& r: rucksacks) {
        if (group_index == 0) first = r.first + r.second; 
        else if (group_index == 1) second = r.first + r.second; 
        else {
            third = r.first + r.second; 
            total_priority += compare_group(first, second, third); 
            group_index = 0; 
            continue; 
        }
        group_index += 1; 
    }
    return total_priority; 
}

void solve_part_two(const std::vector<Rucksack>& rucksacks) {
    auto priority = calculate_priorities_by_group(rucksacks); 
    std::cout << "The solution to part two is: " << priority << std::endl; 
}