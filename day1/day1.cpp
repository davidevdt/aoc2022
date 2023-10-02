#include <vector> 
#include <iostream> 
#include <cstring> 
#include <numeric>
#include <sstream>
#include <string> 
#include <fstream> 
#include <algorithm>

std::vector<std::vector<int>> elf_calories; 
bool is_test{false};  

void get_data(const std::string&, std::vector<std::vector<int>>&); 
void solve_part_one(const std::vector<std::vector<int>>&); 
void solve_part_two(const std::vector<std::vector<int>>&); 
auto find_largest_n(const std::vector<std::vector<int>>&, size_t ); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 1 <<<" << std::endl; 

    std::string file_name{}; 

    if (argc > 1 && std::strcmp(argv[1], "test") == 0) {
        is_test = true; 
    }

    if (is_test) {
        file_name = "./test.txt"; 
        // std::cout << input_string << std::endl; 
    } else {
        file_name = "./input.txt"; 
    }

    get_data(file_name, elf_calories); 

    if (is_test) {
        for (auto e: elf_calories) {
            for (auto c: e) {
                std::cout << c  << " " ;
            }
            std::cout << "\n"; 
        }
    }

    solve_part_one(elf_calories); 
    solve_part_two(elf_calories); 
    
    return 0; 

}

void get_data(const std::string& file_name, std::vector<std::vector<int>>& elf_data) {

    std::ifstream input_file(file_name); 
    // std::istringstream data_stream(data_string); 
    std::string line; 
    int calories; 
    std::vector<int> elf;

    while (std::getline(input_file, line)) {
        // std::cout << line << std::endl; 
        if (line == "\n" || line == "") {
            if (elf.size() > 0) {
                elf_data.push_back(elf); 
                elf.clear(); 
                // elf = std::vector<int>{}; 
            }
            continue; 
        }
        std::istringstream(line) >> calories; 
        elf.push_back(calories); 
    }

    elf_data.push_back(elf); 

    input_file.close(); 

}

auto find_largest_n(const std::vector<std::vector<int>>& elf_calories, size_t n) {
    std::vector<int> total_elf_calories; 
    for (const auto& elf: elf_calories) {
        total_elf_calories.push_back(std::accumulate(elf.begin(), elf.end(), 0)); 
    }
    std::sort(total_elf_calories.begin(), total_elf_calories.end(), [](int e1, int e2) {
        return e1 > e2; 
    }); 

    int sum_first_n = 0; 
    for (size_t i = 0; i < n; i++) {
        sum_first_n += total_elf_calories.at(i); 
    }
    return sum_first_n; 
}

void solve_part_one(const std::vector<std::vector<int>>& elf_calories) { 
    // int max_calories = 0; 
    // for (const auto& elf: elf_calories) {
    //     auto current_calories = std::accumulate(elf.begin(), elf.end(), 0); 
    //     if (current_calories > max_calories) max_calories = current_calories; 
    // }
    auto max_calories = find_largest_n(elf_calories, 1); 
    std::cout << "The answer of part one is: " << max_calories << std::endl; 
}

void solve_part_two(const std::vector<std::vector<int>>& elf_calories) { 
    auto max_calories = find_largest_n(elf_calories, 3); 
    std::cout << "The answer of part two is: " << max_calories << std::endl; 
}

