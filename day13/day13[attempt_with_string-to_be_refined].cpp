#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 

using PacketPair = std::pair<std::string, std::string>; 

void get_data(const std::string&, std::vector<PacketPair>&); 
void solve_part_one(const std::vector<PacketPair>& signals); 
// void solve_part_two(Grid& grid); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 13 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<PacketPair> signals; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, signals); 

    if (is_test) {
        for (const auto& s: signals) {
            std::cout << s.first << std::endl; 
            std::cout << " VS " << std::endl; 
            std::cout << s.second << std::endl; 
            std::cout << "-----------" << std::endl; 
        }
    }

    solve_part_one(signals); 
    // solve_part_two(); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<PacketPair>& signals) {
    std::ifstream input_file{file_name}; 
    if (input_file.is_open()) {
        std::string line; 
        std::vector<std::string> tmp_strings; 
        while (std::getline(input_file, line)) {
            if (line.empty()) continue; 
            tmp_strings.push_back(line); 
            if (tmp_strings.size() == 2) {
                PacketPair signal_pair = PacketPair(tmp_strings.at(0), tmp_strings.at(1)); 
                signals.push_back(signal_pair); 
                tmp_strings.clear();
            } 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

unsigned int get_number(std::string::const_iterator& it) {
    std::string str_number; 
    while (std::isdigit(*it)) {
        str_number.push_back(*it); 
        ++it; 
    }
    auto number = std::stoul(str_number); 
    // if (*it == ',') ++it; 
    return number; 
}

auto compare_numbers(std::string::const_iterator& it1, std::string::const_iterator& it2) -> int {
    auto n1 = get_number(it1), n2 = get_number(it2); 
    if (n1 > n2) return -1; 
    else if (n1 < n2) return 1; 
    else return 0; 
}

auto compare_pair(std::string::const_iterator& it1, std::string::const_iterator& it2, const std::string::const_iterator& end1, const std::string::const_iterator& end2) -> int {
    bool is_right_order = false; 

    while (true) {
        std::cout << "it1: " << *it1 << ", it2: " << *it2 << std::endl; 

        if (it1 == end1 && it2 == end2 ) return 0; 
        else if (it1 == end1) return 1; 
        else if (it2 == end2) return -1; 

        // Either of the character is ',' -> advance the cursors 
        if (*it1 == ',' && *it2 == ',') {
            ++it1; ++it2; continue; 
        } else if (*it1 == ',') {++it1; continue;} 
        else if (*it2 == ',') {++it2; continue; }

        // Both characters are digits -> compare them
        if (std::isdigit(*it1) && std::isdigit(*it2) ) {
            auto result = compare_numbers(it1, it2); 
            if (result != 0) return result; 
            else continue; 
        }
        else if (std::isdigit(*it1)) {
            if (*it2 == ']') return -1;
            if (*it2 == '[') ++it2; continue; 
        }
        else if (std::isdigit(*it2)) {
            if (*it1 == ']') return 1; 
            if (*it1 == '[') ++it1; continue; 
        }

        // Both characters start a list -> advance both cursors 
        else if (*it1 == '[' && *it2 == '[') {
            ++it1; ++it2; 
            continue; 
        }
        // Only the first character starts at list -> advance its cursor 
        else if (*it1 == '[') {
            auto result = compare_pair(++it1, it2, end1, end2); 
            if (result != 0) return result; 
            else continue; 
        }
        else if (*it2 == '[') {
            auto result = compare_pair(it1, ++it2, end1, end2); 
            if (result != 0) return result; 
            else continue; 
        }
        else if (*it1 == ']' && *it2 != ']') {
            if (*it2 == '[') return -1; 
            if (std::isdigit(*it2)) {
                if (*(it2+1) == ',') return -1; 
                auto result = compare_pair(++it1, it2, end1, end2);
                if (result != 0) return result; 
                else continue; 
            }
        }
        else if (*it2 == ']' && *it1 != ']') {
            if (*it2 == '[') return 1; 
            if (std::isdigit(*it2)) {
                if (*(it1+1) == ',') return 1; 
                auto result = compare_pair(++it1, it2, end1, end2);
                if (result != 0) return result; 
                else continue; 
            }
        }

        ++it1; ++it2; 
    }
}

auto find_ordered_pairs(const std::vector<PacketPair>& signals) {
    size_t sum_correct_pairs{0}; 
    for (size_t i = 0; i < signals.size(); ++i) {

        const std::string& first = signals.at(i).first; 
        const std::string& second = signals.at(i).second; 
        std::cout << "first: " << first <<std::endl;
        std::cout << "second: " << second <<std::endl;
        auto it1 = first.cbegin();     
        auto it2 = second.cbegin(); 
        const auto end1 = first.cend(); 
        const auto end2 = second.cend(); 
        auto result = compare_pair(it1, it2, end1, end2); 
        std::cout << "result: " << result << std::endl; 
        if (result > 0) sum_correct_pairs += (i+1);   
    }
    return sum_correct_pairs; 
}

void solve_part_one(const std::vector<PacketPair>& signals) {
    auto solution = find_ordered_pairs(signals); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// void solve_part_two(Grid& grid) {
//     auto solution = find_size_best_path(grid, 'a'); 
//     std::cout << "The solution to part two is " << solution << std::endl; 
// }


