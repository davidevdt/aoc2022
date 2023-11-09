#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <algorithm>
#include <regex>  
#include <unordered_map> 
#include <unordered_set> 
#include <limits> 
#include <utility> 
#include <set> 

// For part 1, I will simulate the rocks dropping, one by one. The air jets will define horizontal directions, 
// followed by a downwards one (for each rock we just need to start from the highest current level + 1). 
// For part 2, we need to identify a circular cycle in order to speed up the computations. This is done by caching 
// the resuts of all potential cycles for the following combinations: 
// (relative heights at each chamber column - current rock - current jet air)

using Coordinate = std::pair<int, int>; 
using Rock = std::vector<Coordinate>; 

Rock CrossRock = {{2, 1}, {3, 1}, {3, 2}, {3, 0}, {4, 1}};
Rock VerticalRock = {{2, 0}, {2, 1}, {2, 2}, {2, 3}}; 
Rock HorizontalRock = {{2, 0}, {3, 0}, {4, 0}, {5, 0}}; 
Rock LRock = {{2, 0}, {3, 0}, {4, 0}, {4, 1}, {4, 2}}; 
Rock SquareRock = {{2, 0}, {2, 1}, {3, 0}, {3, 1}}; 
std::vector<Rock> rocks = {HorizontalRock, CrossRock, LRock, VerticalRock, SquareRock}; 

// Useful to create a hash set of Rocks (to store which rocks we have in the chamber)
struct CoordinateHash {
    size_t operator()(const Coordinate& c) const {
        auto h1 = std::hash<int>{}(c.first); 
        auto h2 = std::hash<int>{}(c.second); 
        return h1 ^ h2; 
    }
}; 

// Used for debugging
// auto rock_to_string(const Rock& rock) {
//     std::string r_string = ""; 
//     for (const auto& coordinate: rock) {
//         r_string += "(" + std::to_string(coordinate.first) + "," + std::to_string(coordinate.second) + ") "; 
//     }
//     r_string +="\n"; 
//     return r_string; 
// }

// Moves rock downwards, upwards, to the left or right. If move_type = "horizontal": then a negative number moves to the left, 
// and a positive one to the right. If move_type = "vertical": then a negative number moves down, and a positive one up. 
// It returns false if the rock could not move (either because it's in the chamber's edges or because its going to go to 
// an already occupied location in the chamber), in which case the rock does not move at all. 
auto move_rock(Rock& rock, int n_steps, const std::unordered_set<Coordinate, CoordinateHash>& chamber, std::string move_type = "horizontal") {

    auto tmp_rock = rock; 

    if (move_type == "vertical") {
        for (auto& coordinate: tmp_rock) {
            coordinate.second += n_steps; // when we pass a positive number, we move the rock upwards
            if (coordinate.second == 0 || chamber.find(coordinate) != chamber.end()) return false; 
        }  
    }
    else {
        // If the rock is on the edge, don't move it 
        if (n_steps < 0 && tmp_rock.at(0).first == 0) return false; 
        if (n_steps > 0 && tmp_rock.at(tmp_rock.size()-1).first == 6) return false; 

        for (auto& coordinate: tmp_rock) {
            coordinate.first += n_steps; // when we pass a negative number, we go leftwards
            if (chamber.find(coordinate) != chamber.end()) return false;  
        }
    }
    rock = tmp_rock; // If the move is valid, updates the rock coordinates
    return true; 
}

void get_data(const std::string&, std::string&);  
void solve_part_one(const std::string&); 
void solve_part_two(const std::string&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 17 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::string jets; 


    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, jets); 

    if (is_test) {
        std::cout << jets << std::endl; 
    }

    solve_part_one(jets); 
    solve_part_two(jets); 

    return 0; 
}

// Get the data, using regex 
void get_data(const std::string& file_name, std::string& jets) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            jets += line; 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// Drop the rocks one by one 
auto drop_rocks(const std::string& jets) {
    
    int chamber_highest_level = 0; 
    std::vector<int> chamber_col_highest_level(7, 0);
    size_t jet_index = 0; 
    std::unordered_set<Coordinate, CoordinateHash> chamber; 

    for (size_t i = 0; i < 2022; ++i) {
        bool is_at_rest = false; 
        Rock rock = rocks.at(i % 5);
        move_rock(rock, chamber_highest_level + 1, chamber, "vertical");
        // In a first round, we know for sure that the rock needs to go down at least 4 times, 
        // which corresponds to 4 horizontal movements, before potentially touching either the floor 
        // or one of the resting rocks. 
        for (size_t first_round = 0; first_round < 4; ++first_round) {
            auto horizontal_step_direction = jets.at(jet_index) == '>' ? 1 : -1; 
            move_rock(rock, horizontal_step_direction, chamber);
            jet_index = (jet_index + 1) % jets.size();  
        }

        // If the rock could not move, then it is at rest
        is_at_rest = !move_rock(rock, -1, chamber, "vertical"); 

        // If after the first 4 steps the rock is still not at rest, we continue going down until the rock comes at rest. 
        while (!is_at_rest) { 
            auto horizontal_step_direction = jets.at(jet_index) == '>' ? 1 : -1; 
            move_rock(rock, horizontal_step_direction, chamber);
            jet_index = (jet_index + 1) % jets.size();
            // If the rock could not move, then it is at rest
            is_at_rest = !move_rock(rock, -1, chamber, "vertical"); 
        }
        // Updates the various heights for each chamber column, and update the maximum height
        if (is_at_rest) {
            for (const auto& coordinate: rock) {
                chamber.insert(coordinate); 
                chamber_col_highest_level.at(coordinate.first) = 
                    coordinate.second > chamber_col_highest_level.at(coordinate.first) ?
                        coordinate.second : chamber_col_highest_level.at(coordinate.first);   
                chamber_highest_level = chamber_col_highest_level.at(coordinate.first) > chamber_highest_level ? 
                    chamber_col_highest_level.at(coordinate.first) : chamber_highest_level; 
            }
        }
    }
    return chamber_highest_level; 
}        

void solve_part_one(const std::string& jets) {
    auto solution = drop_rocks(jets);
    std::cout << "The solution to part two is " << solution << std::endl; 
}

// The algorithm works as in part one, except that we keep track in a cache of the following combinations: 
// (relative rock heights at the top row - rock cycle # (1,...,5) - airjet move) 
// If at some point of the iterations we encounter this cycle, we update the rock numbers and the gain in max height
auto drop_rocks_with_cycles(const std::string& jets, size_t n_rocks) {
    
    long int chamber_highest_level = 0; 
    std::vector<long int> chamber_col_highest_level(7, 0);
    size_t jet_index = 0; 
    std::unordered_set<Coordinate, CoordinateHash> chamber; 
    long int rock_counter = 0; 
    std::unordered_map<std::string, std::vector<long int>> cache; 
    bool cycle_found = false; 
    long int number_of_cycles = 0; 
    long int height_gain = 0; 

    while (rock_counter < n_rocks) {
        bool is_at_rest = false; 
        Rock rock = rocks.at(rock_counter % 5);
        move_rock(rock, chamber_highest_level + 1, chamber, "vertical");
        for (size_t first_round = 0; first_round < 4; ++first_round) {
            auto horizontal_step_direction = jets.at(jet_index) == '>' ? 1 : -1; 
            move_rock(rock, horizontal_step_direction, chamber);
            jet_index = (jet_index + 1) % jets.size();  
        }

        is_at_rest = !move_rock(rock, -1, chamber, "vertical"); 

        while (!is_at_rest) { 
            auto horizontal_step_direction = jets.at(jet_index) == '>' ? 1 : -1; 
            move_rock(rock, horizontal_step_direction, chamber);
            jet_index = (jet_index + 1) % jets.size();

            is_at_rest = !move_rock(rock, -1, chamber, "vertical"); 
        }
        if (is_at_rest) {
            for (const auto& coordinate: rock) {
                chamber.insert(coordinate); 
                chamber_col_highest_level.at(coordinate.first) = 
                    coordinate.second > chamber_col_highest_level.at(coordinate.first) ?
                        coordinate.second : chamber_col_highest_level.at(coordinate.first);   
                chamber_highest_level = chamber_col_highest_level.at(coordinate.first) > chamber_highest_level ? 
                    chamber_col_highest_level.at(coordinate.first) : chamber_highest_level; 
            }
        }

        // If we did not compete the cycle yet... (else, if the cycle is found, completes the loop until n_rocks in total
        // have been dropped)
        if (!cycle_found) {
            // Create cache key 
            std::string chamber_from_top_str = ""; 
            for (const auto& c: chamber_col_highest_level) chamber_from_top_str += \
                std::to_string(c - chamber_highest_level) + "-"; 
            chamber_from_top_str += std::to_string(rock_counter % 5) + "-" + std::to_string(jet_index);
            // If the current combination is in the cache, we are in the cycle and can update the cycle quantities 
            if (cache.find(chamber_from_top_str) != cache.end()) {
                height_gain = chamber_highest_level - cache[chamber_from_top_str].at(0); 
                auto cycle_length = rock_counter - cache[chamber_from_top_str].at(1); // the number of rocks in the cycle
                number_of_cycles = (n_rocks - rock_counter) / cycle_length; // top the number of rocks at n_rocks
                rock_counter += number_of_cycles * cycle_length; 
                cycle_found = true; 
            } else { // otherwise store the quantities in cache for future potential cycle with this key combination
                cache[chamber_from_top_str] = {chamber_highest_level, rock_counter}; 
            }
        }
        ++rock_counter; 

    }
    return chamber_highest_level + (number_of_cycles * height_gain); 
}        

void solve_part_two(const std::string& jets) {
    auto solution = drop_rocks_with_cycles(jets, 1000000000000);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


