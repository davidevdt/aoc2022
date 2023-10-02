#include <iostream> 
#include <vector> 
#include <fstream>
#include <string>
#include <sstream>
#include <cstdint> 
#include <stdlib.h>

struct Section {
    uint16_t firstElfStart; 
    uint16_t firstElfEnd; 
    uint16_t secondElfStart; 
    uint16_t secondElfEnd; 

    Section(uint16_t fStart, uint16_t fEnd, uint16_t sStart, uint16_t sEnd): 
        firstElfStart(fStart), firstElfEnd(fEnd), 
            secondElfStart(sStart), secondElfEnd(sEnd) {}
}; 

void get_data(const std::string&, std::vector<Section>&); 
void solve_part_one(const std::vector<Section>&); 
void solve_part_two(const std::vector<Section>&); 

int main (int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 4 <<<" << std::endl; 

    std::string file_name; 
    std::vector<Section> assignments; 
    bool is_test = false; 

    if (argc > 1 && std::string(argv[1]) == "test") {
        is_test = true; 
    }

    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, assignments);

    if (is_test) {
        for (const auto& a: assignments) {
            std::cout << "First elf start: " << a.firstElfStart << ", first elf end: " <<
            a.firstElfEnd << " // Second elf start: " << a.secondElfStart << ", second elf end: " <<
            a.secondElfEnd <<  "\n" << 
            "-----------------------------------------------------------------------------" << std::endl; 
        }
    }

    solve_part_one(assignments); 
    solve_part_two(assignments); 


}

void get_data(const std::string& file_name, std::vector<Section>& assignments) {

    std::ifstream input_file(file_name); 

    if (input_file.is_open()) {
        std::string line; 
        std::string elves; 
        std::istringstream elf_stream;  
        while (std::getline(input_file, line)) {
            std::istringstream line_stream{line};
            std::vector<uint16_t> input_assignments;
            input_assignments.reserve(4);  
            uint16_t elfStart, elfEnd;
            while (std::getline(line_stream,  elves, ',')) {
                std::istringstream elf_stream{elves};
                elf_stream >> elfStart; 
                elf_stream.ignore(); 
                elf_stream >> elfEnd; 
                input_assignments.push_back(elfStart);
                input_assignments.push_back(elfEnd); 
            }
            assignments.emplace_back(input_assignments[0], input_assignments[1], input_assignments[2], input_assignments[3]);
        }

    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 
} 

auto assignments_is_contained(const Section& section) {
    auto first_elf_range = section.firstElfEnd - section.firstElfStart; 
    auto second_elf_range = section.secondElfEnd - section.secondElfStart; 

    if (first_elf_range > second_elf_range) {
        return section.firstElfStart <= section.secondElfStart && 
                section.firstElfEnd >= section.secondElfEnd; 
    }
    
    return section.secondElfStart <= section.firstElfStart && 
                section.secondElfEnd >= section.firstElfEnd;  
}

auto count_containing_assignments(const std::vector<Section>& assignments) {
    int count = 0; 
    for (const auto& a: assignments) {
        count += assignments_is_contained(a); 
    }
    return count; 
}

void solve_part_one(const std::vector<Section>& assignments) {
    auto count = count_containing_assignments(assignments); 
    std::cout << "The solution to part one is: " << count << std::endl; 
}

auto assignment_is_overlapping(const Section& section) {

    if (section.firstElfStart < section.secondElfStart) {
        return section.secondElfStart <= section.firstElfEnd;
    } 

    if (section.secondElfStart < section.firstElfStart) {
        return section.firstElfStart <= section.secondElfEnd; 
    }

    // the base condition is when the two assignments begin at the same slot
    // in which case the overlap is automatically true
    return true; 
}

auto count_overlapping_assignments(const std::vector<Section>& assignments) {
    int count = 0; 
    for (const auto& a: assignments) {
        count += assignment_is_overlapping(a); 
    }
    return count; 
}

void solve_part_two(const std::vector<Section>& assignments) {
    auto count = count_overlapping_assignments(assignments); 
    std::cout << "The solution to part two is: " << count << std::endl; 
}