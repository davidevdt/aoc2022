#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream> 
#include <cstdint> 
#include <stdlib.h>

struct Move {
    uint16_t size; 
    uint16_t from; 
    uint16_t to; 

    Move(uint16_t s, uint16_t f, uint16_t t): size(s), from(f), to(t) {}
};

void get_data(const std::string&, std::vector<std::string>&, std::vector<Move>&); 
void solve_part_one(const std::vector<std::string>&, const std::vector<Move>&); 
void solve_part_two(const std::vector<std::string>&, const std::vector<Move>&); 

int main (int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 5 <<<" << std::endl; 

    std::string file_name{}; 
    std::vector<std::string> stacks; 
    std::vector<Move> moves; 
    bool is_test{false}; 

    if (argc > 1 && std::string(argv[1]) == "test") {
        is_test = true; 
    }

    if (is_test) {
        file_name = "./test.txt"; 
    } else {
        file_name = "./input.txt"; 
    }

    get_data(file_name, stacks, moves); 

    if (is_test) {
        std::cout << "INITIAL STACK: " << std::endl; 
        for (const auto& s: stacks) {
            std::cout << s << std::endl; 
        }
        std::cout << "-------------------------------" << std::endl; 
        std::cout << "MOVES: " << std::endl; 
        for (const auto& m: moves) {
            std::cout << "Size:" << m.size << ", from: " << m.from << ", to: " << m.to << std::endl; 
        }
        std::cout << "-------------------------------" << std::endl; 
    }

    solve_part_one(stacks, moves); 
    solve_part_two(stacks, moves); 

}

void get_data(const std::string& file_name, std::vector<std::string>& stacks,
                std::vector<Move>& moves) {

    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        bool getting_configurations{true}; 
        
        while (std::getline(input_file, line)) {
            auto n = line.size(); 
            if (n > 0 && line.at(1) == '1' ) {
                getting_configurations = false; 
                continue; 
            }

            if (n == 0 && !getting_configurations) continue; 

            // Part 1: Get current stack configurations 
            if (getting_configurations) {
                unsigned int crate_counter{0}; 
                uint16_t crate_it{0}; // from 0 to 3: "[N] "
                for (const auto& line_char: line) {
                    if (crate_counter >= stacks.size()) {
                        stacks.push_back(""); 
                    } 
                    if (crate_it == 1 && line_char != ' ') {
                        stacks.at(crate_counter) += line_char; 
                    } 
                    else if (crate_it == 3) {   // Increment and iterate through new crate 
                        crate_counter++; 
                        crate_it = 0;
                        continue;  
                    }
                    crate_it++; 
                }
            } 
            else {      // Part 2: get moves 
                std::istringstream move_stream{line}; 
                std::string to_ignore{}; 
                uint16_t size, from, to; 
                move_stream >> to_ignore; // "move"
                move_stream >> size; 
                move_stream >> to_ignore; // "from"
                move_stream >> from; 
                move_stream >> to_ignore; // "to"
                move_stream >> to; 
                moves.emplace_back(size, from-1, to-1); // Subtract 1 to conform to c++ indexing 
            }          
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
}

// Note: stacks is passed-by-valued as we need fresh input data for part two 
auto move_crates(std::vector<std::string> stacks, const std::vector<Move>& moves, bool part_one=true) {
    std::ostringstream answer; 
    for (const auto& move: moves) {
        const auto& size = move.size; 
        const auto& from = move.from; 
        const auto& to = move.to; 
        auto& source_crate = stacks.at(static_cast<size_t>(from)); 
        auto& destination_crate = stacks.at(static_cast<size_t>(to)); 
        auto moved_part = source_crate.substr(0, size); 
        source_crate = source_crate.substr(size, source_crate.size()); 
        if (part_one) { // Part one: reversed order (one item at a time)
            for (auto& c: moved_part) {
                destination_crate = c + destination_crate; 
            }
        } 
        else {  // Part two: order is preserved
            destination_crate = moved_part + destination_crate; 
        }
    }
    for (const auto& s: stacks) {
        answer << s.at(0); 
    }
    return answer.str(); 
}

void solve_part_one(const std::vector<std::string>& stacks, const std::vector<Move>& moves) {
    auto answer = move_crates(stacks, moves); 
    std::cout << "The solution to part one is: " << answer << std::endl; 
}

void solve_part_two(const std::vector<std::string>& stacks, const std::vector<Move>& moves) {
    auto answer = move_crates(stacks, moves, false); 
    std::cout << "The solution to part two is: " << answer << std::endl; 
}
