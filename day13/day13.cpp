#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <algorithm> 

/* This has been the hardest puzzle so far. It took me quite some time to understand how to parse the 
signals and into which data structure to store them, let alone thinking about all the possible cases 
in which they can be compared. Eventually, with the help of Python (whose script can be found in this directory), 
by means of which finding the right solution was pretty straightforward and quick (Python script written only for Part 1). 

Eventually, I could find the solution in c++ too as follows: 
- The signals are stored in a data structure called NestedVector, which contains a vector (called list) of other NestedVector's
  and a vector (called elements) of unsigned int's; if the list is empty, then the elements vector contains number, while 
  if the list is not empty then the elements vector is. This could have been more efficient by using for example a 
  union or a std::variant 
- The file is parsed line by line, and the function that parses the elements is called recursively in case of a vector
  nested inside another one. If there are some isolated elements (which are recognized if they appear between a ']' and 
  another ']', or between a '[' and another '[', or between  a ']' and a '['), then they are inserted in the "parent" 
  NestedNode's list vectors as a NestedNode object with a single number in their elements list
- The CompairPair function takes into account all possible cases -- see its comments for more details. */

// The NestedVector class 
struct NestedVector {
    std::vector<NestedVector> list; 
    std::vector<unsigned int> elements;

    // Print function that helps visualize the created NestedVector (recursive calls to check inner vectors)
    void print(int depth = 0) const {
        std::cout << " [";
        if (!list.empty()) {
            for (const auto& l: list) {
                l.print(1); 
            }
        } else {
            for (auto it = elements.begin(); it != elements.end(); ++it) {
                std::cout << *it; 
                if (std::next(it) != elements.end()) {
                    std::cout << ",";
                } 
            } 
        }
        std::cout << "] "; 
        if (depth == 0) std::cout << std::endl; 
    } 
}; 

using PacketPair = std::pair<NestedVector, NestedVector>; 

void get_data(const std::string&, std::vector<PacketPair>&); 
void solve_part_one(std::vector<PacketPair>&); 
void solve_part_two(const std::vector<PacketPair>& signals); 

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
            s.first.print(); 
            std::cout << " VS " << std::endl; 
            s.second.print(); 
            std::cout << "-----------" << std::endl; 
        }
    }

    solve_part_one(signals); 
    solve_part_two(signals); 

    return 0; 
}

/*
    INPUT PARSER FUNCTIONS 
*/

// Checks if a number must be stored as an isolated NestedVector: 
// this occurs if the first square bracket preceding it is NOT '[' and 
// the next square bracket following it is NOT '[' 
auto vector_is_isolated(const std::string& line, const size_t index) {
    auto tmp_index{index}; 
    char first_open, first_closed; 
    while (first_open != '[' && first_open != ']') {
        first_open = line.at(tmp_index); 
        --tmp_index; 
    }
    tmp_index = index + 1; 
    while (first_closed != '[' && first_closed != ']') {
        first_closed = line.at(tmp_index); 
        ++tmp_index; 
    }
    if (!(first_open == '[' && first_closed == ']')) return true; 
    return false; 

}

// Parse each line of the input file. The level input (either 0 or 1) is useful to understand whether we are 
// in the first, outer NestedVector level, or if we are inside an inner vector. This is useful to skip the first "["
// so to avoid creating an extra level of children NestedVector's. 
auto parse_nested_vector(std::string& line, int level, size_t& index) -> NestedVector {
    NestedVector signal_vector; 
    bool line_has_completed = false; 
    while (!line_has_completed) {
        char next_char = line.at(index);   
        // Skip comma's 
        if (next_char == ',') {
            ++index; 
            continue; 
        }
        // If we find a '[' and we are parsing the outer NestedVector, 
        // continue; otherwise call the function recursively and push the resulting 
        // Nested vector inside the "list" vector 
        else if (next_char == '[') {
            if (level == 1) { 
                signal_vector.list.push_back(parse_nested_vector(line, 1, ++index)); 
            } else {  
                ++index; 
                level = 1; 
                continue; 
            }
        } 
        // If we are finding a char: check if it must be isolated into its own NestedVector 
        // Then parse the digits until the next character is a digit (for example to parse the 
        // number 10), create a std::vector of such numbers and store them into the elements 
        // vector of the corresponding parent NestedVector 
        else if (std::isdigit(next_char)) {
            auto is_isolated = vector_is_isolated(line, index); 
            std::vector<unsigned int> elements; 
            unsigned int next_number; 
            while (true) {
                std::string str_number;
                while (std::isdigit(next_char)) {
                    str_number.push_back(next_char); 
                    if (index == line.size() - 1) break; 
                    ++index; 
                    next_char = line.at(index);            
                }
                next_number = std::stoul(str_number); 
                elements.push_back(next_number); 
                if (next_char == ',') {
                    if (is_isolated) break; 
                    ++index;
                    next_char = line.at(index); 
                    if (next_char == '[') {
                        ++index; break;
                    } 
                } else if (next_char == ']') break;   
            }
            if (is_isolated) {
                NestedVector new_nested_vector; 
                new_nested_vector.elements = elements; 
                signal_vector.list.push_back(new_nested_vector); 
            } else {
                signal_vector.elements = elements; 
            }
        }
        // When we encounter a ']' either we continue (if the line has not ended)
        // else we return the created NestedVector
        if (line.at(index) == ']') {
            if (index != line.size() - 1) ++index;  
            return signal_vector; 
        }
        // If for some reason we end up at this point, terminate the loop 
        if (index == line.size()) line_has_completed = true; 
    }
    // Return the created vector 
    return signal_vector; 
}

// Parse the input file line by line and create the list of PacketPairs (nested vector vs. nested vector)
// using the parser defined above
void get_data(const std::string& file_name, std::vector<PacketPair>& signals) {
    std::ifstream input_file{file_name}; 
    if (input_file.is_open()) {
        std::string line; 
        std::vector<NestedVector> tmp_vectors; 
        while (std::getline(input_file, line)) {
            if (line.empty()) continue; 
            size_t index = 0;
            auto signal_vector = parse_nested_vector(line, 0, index);
            tmp_vectors.push_back(signal_vector); 
            if (tmp_vectors.size() == 2) {
                PacketPair signal_pair = PacketPair(tmp_vectors.at(0), tmp_vectors.at(1)); 
                signals.push_back(signal_pair); 
                tmp_vectors.clear();
            } 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

/*
    ALGORITHM FUNCTIONS
*/

// Compares std::vectors of integers as required by the instruction: as soon as one number differs from 
// the other, it returns 1 (if the second vector's number is smaller) or -1 (if the first vector's number is smaller)
// If the two vectors have the same number of equal elements, it returns 0; if the elements are the same, but the 
// first vector is larger, it returns -1; else it returns 1 
auto compare_vectors(const std::vector<unsigned int>& first, const std::vector<unsigned int>& second ) {
    auto v1 = first.begin(), v2 = second.begin(); 
    while(true) {
        if (v1 == first.end() && v2 == second.end()) {
            return 0; 
        }
        else if (v1 == first.end() && v2 != second.end()) return 1; 
        else if (v1 != first.end() && v2 == second.end()) return -1;  

        if (*v1 == *v2) {
            ++v1; ++v2; continue; 
        } 
        else if (*v1 > *v2) return -1; 
        else return 1; 
    }
}

// Compares a pair of NestedVectors according to the requirements. All possible cases are 
// analyzed and given in the comments. This function calls itself recursively in case of 
// inner vectors comparison 
auto compare_pair(NestedVector& first, NestedVector& second) -> int {
    // Extract all the elements from each NestedVector 
    // Extract the lists' and elements' sizes 
    auto& e1 = first.elements; auto& e2 = second.elements; 
    auto s1 = e1.size(), s2 = e2.size(); 
    auto n1 = first.list.size(), n2 = second.list.size(); 
    auto& l1 = first.list; auto& l2 = second.list; 
    size_t index1 = 0, index2 = 0; 

    // Case 1: both lists are empty
    if (n1 == 0 && n2 == 0) {
        if (s1 > 0 || s2 > 0) { 
            // Case 1a: the two numeric vectors are populated 
            auto result = compare_vectors(e1, e2);
            return result; 
        }  
        else if (s1 > 0 && s2 == 0) return -1; // Case 1b: only the first inner vector contains numbers
        else if (s1 == 0 && s2 > 0) return 1; // Case 1c: only the second inner vector contains numbers 
        else return 0; // Both vectors are empty 
    } 
    // Case 2: the first NestedVector's list contains more elements than the second
    else if (n1 >= n2) {
        int result; 
        // Case 2a: the second NestedVector does not contain NestedVector's 
        if (n2 == 0) {
            // Case 2ai: the second NestedVector's elements object contains numbers 
            if (s2 > 0) {
                result = compare_pair(l1.at(0), second); 
                if (result != 0) return result; 
                else return -1; // n1 > n2 = 0 
            } 
            else return -1;  // Case 2aii: the second NestedVector's elements object does not contain numbers 
        }

        // Case 2b: List vs. number 
        // (the first NestedVector contains other NestedVectors, but the second one only contains numbers)
        // In this case insert each number into its own NestedVector and compare such nested vector 
        // against the NestedVectors of the first object. Do this one by one 
        // It returns 1 if the comparison is 0 but the second NestedVector input has more elements 
        // than the one it is compared against  
        if (n1 > 1 && n2 == 1 && (l2.at(0).list.size() == 0 && l2.at(0).elements.size() >= 1)) {
            for (size_t i = 0; i < l2.at(0).elements.size(); ++i) {
                NestedVector nv;
                nv.elements =  std::vector<unsigned int>(1, l2.at(0).elements.at(i));
                result = compare_pair(l1.at(i), nv);  
                if (result != 0) return result; 
                if (result == 0 && l2.at(0).elements.size() > l1.at(0).elements.size()) return 1; 
            }
        }

        // Case 2c: list vs. list -> it loops through the lists of the two input NestedVector's
        // std::vector<NestedVector> ll1(l1.begin(), l1.end()); 
        // std::vector<NestedVector> ll2(l2.begin(), l2.end()); 
        for (size_t ind = 0; ind < n2; ++ind) {
            // Case 2ci: The current inner NestedVector of the second input list contains only numbers 
            // (in which case wrap it with a new NestedVector and insert it into the list to allow for recursive call 
            // of the comparison function)
            if (l1.at(ind).list.size() > 0 && l2.at(ind).list.size() == 0 && l2.at(ind).elements.size() > 0) {
                NestedVector nv; 
                nv.list.push_back(l2.at(ind)); 
                l2.at(ind) = nv; 
            }
            // Case 2cii: The current inner NestedVector of the first input list contains only numbers
            // (in which case wrap it with a new NestedVector and insert it into the list to allow for recursive call 
            // of the comparison function)
            else if (l2.at(ind).list.size() > 0 && l1.at(ind).list.size() == 0 && l1.at(ind).elements.size() > 0) {
                NestedVector nv; 
                nv.list.push_back(l1.at(ind)); 
                l1.at(ind) = nv; 
            }
            result = compare_pair(l1.at(ind), l2.at(ind)); 
            if (result != 0) return result; 
        }
        // Case 2d: If all tests so far led to lists' equality, check the lists' size
        if (n1 > n2) return -1;             
        else return 0; // n1 == n2 case 
    }

    // Case 3: the second NestedVector's list contains more elements than the first (n2 > n1)
    // This case is the mirrored version of Case 2 
    int result; 
    if (n1 == 0) {
        if (s1 > 0) {
            result = compare_pair(first, l2.at(0)); 
            if (result != 0) return result;
            else return 1; // n2 > n1 = 0  
        } 
        else return 1; 
    }

    // List vs. number 
    if (n2 > 1 && n1 == 1 && (l1.at(0).list.size() == 0 && l1.at(0).elements.size() >= 1)) {
        for (size_t i = 0; i < l1.at(0).elements.size(); ++i) {
            NestedVector nv;
            nv.elements =  std::vector<unsigned int>(1,l1.at(0).elements.at(i));
            result = compare_pair(nv, l2.at(i));  
            if (result != 0) return result; 
            if (result == 0 && l1.at(0).elements.size() > l2.at(0).elements.size()) return -1; 
        }
    }

    // List vs. list 
    // std::vector<NestedVector> ll1(l1.begin(), l1.end()); 
    // std::vector<NestedVector> ll2(l2.begin(), l2.end());  
    for (size_t ind = 0; ind < n1; ++ind) {
        if (l1.at(ind).list.size() > 0 && l2.at(ind).list.size() == 0 && l2.at(ind).elements.size() > 0) {
            NestedVector nv; 
            nv.list.push_back(l2.at(ind)); 
            l2.at(ind) = nv; 
        }
        else if (l2.at(ind).list.size() > 0 && l1.at(ind).list.size() == 0 && l1.at(ind).elements.size() > 0) {
            NestedVector nv; 
            nv.list.push_back(l1.at(ind)); 
            l1.at(ind) = nv; 
        }
        result = compare_pair(l1.at(ind), l2.at(ind)); 
        if (result != 0) return result; 
    }
    return 1; 
}

// Check the input vector and return the sum of the sorted indices 
auto find_sorted_pairs(std::vector<PacketPair>& signals) {
    size_t sum_sorted_pairs = 0; 
    for (size_t i=0; i<signals.size(); ++i) {
        auto result = compare_pair(signals.at(i).first, signals.at(i).second);  
        if (result > 0) sum_sorted_pairs += (i+1); 
    }
    return sum_sorted_pairs; 
}   

void solve_part_one(std::vector<PacketPair>& signals) {
    auto solution = find_sorted_pairs(signals); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For part two, we write a function that: 
// (a) stores all the NestedVector signals into one vector, rather than in pairs 
// (b) adds the two divider packets into such vector
// (c) uses std::sort to sort the signals according to the compare function we created for Part 1 
auto sort_and_find_divider_indices(const std::vector<PacketPair>& signals) {

    // a. store all signals in a vector
    std::vector<NestedVector> signals_with_dividers; 
    for (const auto& s: signals) {
        signals_with_dividers.push_back(s.first); 
        signals_with_dividers.push_back(s.second); 
    }

    // b. add dividers
    std::vector<unsigned int> first_divider_element(1, 2); 
    std::vector<unsigned int> second_divider_element(1, 6); 
    NestedVector first_divider, second_divider; 
    first_divider.elements = first_divider_element; 
    second_divider.elements = second_divider_element; 
    signals_with_dividers.push_back(first_divider); 
    signals_with_dividers.push_back(second_divider); 

    // c. sort the vector
    std::sort(signals_with_dividers.begin(), signals_with_dividers.end(), [](NestedVector first, NestedVector second){
        return compare_pair(first, second) > 0; 
    }); 

    // d. find the correct indices, multiply them, and return the result 
    size_t num_dividers_found = 0; 
    size_t solution = 1; 
    for (size_t i = 0; i < signals_with_dividers.size(); ++i) {
        auto s = signals_with_dividers.at(i);
        if (s.list.size() == 0 && s.elements.size() == 1 && (s.elements.at(0) == 2 || s.elements.at(0) == 6)) {
            num_dividers_found += 1; 
            solution *= (i+1); 
        }
        if (num_dividers_found == 2) break; 
    }
    return solution; 
}

void solve_part_two(const std::vector<PacketPair>& signals) {
    auto solution = sort_and_find_divider_indices(signals); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}


