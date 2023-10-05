#include <string>
#include <iostream>
#include <cstdint>
#include <stdlib.h>
#include <sstream> 
#include <fstream> 
#include <vector> 

void get_data(const std::string&, std::vector<std::string>&); 
void solve_part_one(const std::vector<std::string>&, bool);
void solve_part_two(const std::vector<std::string>&, bool); 

int main(int argc, char** argv) {

    std::cout << ">>> Advent Of Code 2022 - Day 6 <<<" << std::endl; 

    std::string file_name; 
    bool is_test = false; 
    std::vector<std::string> datastreams; // A vector ain't really necessary here, but the test consists of several strings

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 

    if (is_test) {
        file_name = "./test.txt"; 
        datastreams.reserve(4); // 4 test cases
    }
    else {
        file_name = "./input.txt"; 
        datastreams.reserve(1); 
    }

    get_data(file_name, datastreams); 

    if (is_test) {
        for (const auto& d: datastreams) {
            std::cout << d << std::endl; 
        }
    }

    solve_part_one(datastreams, is_test); 
    solve_part_two(datastreams, is_test); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<std::string>& datastreams) {

    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string datastream; 

        while (std::getline(input_file, datastream)) {
            datastreams.push_back(datastream);
        } 
    } 
    else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

}

// Approach: set a target difference (3 in case of part 1, given by 4 distinct characters -1 
// as we are also counting 0, and 13 in case of part 2 )
// Then loop through the string with two pointers: if they don't match, increase the 
// second pointer until their difference reaches the value of target_difference; 
// then increase the first pointer by 1, decrease the second pointer to index1 + 1 
// and continue the comparison, this time decreasing the target difference. 
// If the target difference reaches 0, return the second pointer. Else, if the two 
// characters match, reset the target difference and place the two pointers in such a way
// to compare a new string 
auto find_marker_index(const std::string& datastream, const size_t n_distinct_characters=4) {
    uint16_t move{0};
    size_t index1 = 0; 
    size_t index2 = 1; 
    size_t target_difference = n_distinct_characters - 1;

	while (target_difference > 0 && index2 < datastream.size()) {
		auto diff = index2 - index1; 
		if (datastream.at(index1) != datastream.at(index2)) {
			if (diff == target_difference) {
				target_difference --; 
				index1 ++;
				index2 = index1 + 1; 
			} else {
				index2 ++; 
			}
		} else {
			target_difference = n_distinct_characters - 1; 
			index1 ++; 
            index2 = index1 + 1; 
        }
	}
	return index2; 
}

void solve_part_one(const std::vector<std::string>& datastreams, bool is_test) {
    if (!is_test) {
        auto marker_index = find_marker_index(datastreams.at(0)); 
        std::cout << "The solution to part one is " << marker_index << std::endl; 
    } 
    else {
        size_t i = 0; 
        for (const auto& s: datastreams) {
            auto marker_index = find_marker_index(s); 
            std::cout << "The solution to part one (test " << i << ") is " << marker_index << std::endl; 
            i++; 
        }
    }
}

void solve_part_two(const std::vector<std::string>& datastreams, bool is_test) {
    if (!is_test) {
        auto marker_index = find_marker_index(datastreams.at(0), 14); 
        std::cout << "The solution to part two is " << marker_index << std::endl; 
    } 
    else {
        size_t i = 0; 
        for (const auto& s: datastreams) {
            auto marker_index = find_marker_index(s, 14); 
            std::cout << "The solution to part two (test " << i << ") is " << marker_index << std::endl; 
            i++; 
        }
    }
}



