#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <unordered_map>
#include <algorithm>

// The map will be stored in a vector of strings, while the instructions will be stored in a union 
// which will alternate storing a number of steps or new directions. The algorithm will move us across
// the grid (= the map) step by step. For Part 1, wrapping around the map will be done by simpling 
// walking backward until the opposite edge of the map is reached. For part 2, the cube will be stitched 
// by changing side and direction according to the cube shape (note that there are two different configurations 
// for test cases and official puzzle).  

using Coordinate = std::pair<size_t, size_t>; 

union Instruction {
    char direction; 
    int forward_steps; 
}; 

void get_data(const std::string&, std::vector<std::string>&, std::vector<Instruction>&);  
void solve_part_one(const std::vector<std::string>&, const std::vector<Instruction>&); 
void solve_part_two(const std::vector<std::string>&, const std::vector<Instruction>&, bool); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 22 <<<" << std::endl; 

    std::string file_name; 
    bool is_test = false; 
    std::vector<std::string> grid;
    std::vector<Instruction> instructions;  

    
    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, grid, instructions); 

    if (is_test) { 
        for (const auto& line: grid) {
            std::cout << line << std::endl; 
        }
        std::cout << ">>>>>>>>> INSTRUCTIONS: <<<<<<<<<<" << std::endl; 
        for (size_t i = 0; i < instructions.size(); ++i) {
            if (i % 2 == 0) std::cout << instructions.at(i).forward_steps;
            else std::cout << instructions.at(i).direction; 
            if (i < instructions.size() - 1) std::cout << " -> ";
            else std::cout << std::endl;  
        }
    
    }

    solve_part_one(grid, instructions); 
    solve_part_two(grid, instructions, is_test); 

    return 0; 
}

// Get the input data
void get_data(const std::string& file_name, std::vector<std::string>& grid, std::vector<Instruction>& instructions) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        bool map_is_done = false; 
        bool is_direction = false; 
        while (std::getline(input_file, line)) {
            
            if (line.empty()) {
                map_is_done = true; 
                continue; 
            }

            // Store the next map line 
            if (!map_is_done) {
                grid.push_back(line); 
                continue; 
            }

            // Import instructions 
            std::istringstream line_stream(line); 

            while (line_stream.peek() != EOF) {
                Instruction instruction; 
                if (!is_direction) {
                    int n_steps; 
                    line_stream >> n_steps; 
                    instruction.forward_steps = n_steps; 
                    is_direction = true; 
                } else {
                    char new_direction = line_stream.get(); 
                    instruction.direction = new_direction; 
                    is_direction = false; 
                }
                instructions.push_back(instruction); 
            }
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// Changes the direction in the map based on the current direction and 
// the turn type required by the instruction - Note that the turn 
// type can only be either Left or Right
auto change_direction(char current_direction, char turn_type) {
    char new_direction; 
    switch (current_direction) {
        case 'R': 
            turn_type == 'R' ?  new_direction = 'D' :  new_direction = 'U'; 
            break; 
        case 'L': 
            turn_type == 'R' ? new_direction = 'U' : new_direction = 'D'; 
            break; 
        case 'U': 
            turn_type == 'R' ? new_direction = 'R' : new_direction = 'L'; 
            break; 
        default: // we're going down 
            turn_type == 'R' ? new_direction = 'L' : new_direction = 'R'; 
    }
    return new_direction; 
}

// Moves from the current position in the map based on the required direction 
// (it only performs one step) 
auto move(Coordinate current_position, char direction) {
    switch (direction) {
        case 'U': 
            --current_position.first; 
            break; 
        case 'D':
            ++current_position.first; 
            break; 
        case 'L':
            --current_position.second;
            break; 
        default: // we are going right 
            ++current_position.second;
            break; 
    }
    return current_position; 
}

// Checks if the input position is on an edge position in the grid
// This can occur in either of the following ways: 
// - if we are moving left or right, we could be at the left/right extremes of a square (or of the whole grid)
// - if we are moving up or down, we could be at the up/down extremes of a square (or of the whole grid)
// - if we are moving up or down, we could also end up in a region without storage (not even white spaces are stored ' ')
auto is_on_edge(const std::vector<std::string>& grid, Coordinate current_position, char direction) {

    if (direction == 'L') {
        if (current_position.second == 0) return true; 
        else return grid.at(current_position.first).at(current_position.second - 1) == ' '; 
    } 
    else if (direction == 'R') {
        if (current_position.second == grid.at(current_position.first).size()-1) return true; 
        else return grid.at(current_position.first).at(current_position.second + 1) == ' '; 
    }
    else if (direction == 'U') {
        if (current_position.first == 0) return true; 
        else return grid.at(current_position.first - 1).size() <= current_position.second || 
                    grid.at(current_position.first - 1).at(current_position.second) == ' '; 
            
    }
    else {  // D = downward direction 
        if (current_position.first == grid.size() - 1) return true; 
        else return grid.at(current_position.first + 1).size() <= current_position.second || 
                    grid.at(current_position.first + 1).at(current_position.second) == ' '; 
            
    }
    
}

// Wrap the grid to move in part one - Performs backward steps in the same row/column until we 
// get to the edge of the grid 
auto wrap_grid(const std::vector<std::string>& grid, const Coordinate current_position, char direction) {
    auto new_coordinate = current_position; 
    auto opposite_direction = direction == 'L' ? 'R' : (
        direction == 'R' ? 'L' : 
            direction == 'U' ? 'D' : 'U'
    ); 

    while (!is_on_edge(grid, new_coordinate, opposite_direction)) { 
        new_coordinate = move(new_coordinate, opposite_direction); 
    }

    return new_coordinate; 

}

// For part two, we wrap the cube and up in the correct cube surface - Note that we also need to flip the 
// indices within the cubes
auto wrap_cube(const std::vector<std::string>& grid, const Coordinate current_position, char& direction, bool is_test) {
    
    auto old_dir = direction; 

    size_t face_size = 50;  // Production: 50 entries per square
    if (is_test) face_size = 4;  // Test: 4 entries per square 

    // First, identify in which of the six faces we are (imagining a matrix of six squares for the cube surfaces)
    auto curr_cube_face_row_number = current_position.first / face_size; 
    auto curr_cube_face_col_number = current_position.second / face_size;
    size_t new_cube_face_row_number, new_cube_face_col_number; 
    
    // Based on the direction and the current cube in which we are positioned, we now try to identify 
    // the correct cube surface (row and column) in which we need to end up, as well as the new direction 
    // into which we need to go -> Note that test and production cases require different configurations
    if (is_test) {
        switch (direction) {
            case 'L': 
                if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 1;
                    direction = 'D';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 1;
                    direction = 'U';
                }
                else if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 3;
                    direction = 'U';
                }
                break; 
            case 'R': 
                if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 3;
                    direction = 'D';
                }
                else if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 3;
                    direction = 'L';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 3) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'L';
                }
                break; 
            case 'U': 
                if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'R';
                }
                else if (curr_cube_face_row_number == 0  && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 0;
                    direction = 'D';
                }
                else if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'D';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 3) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 2;
                    direction = 'L';
                }
                break; 
            default: // case 'D' 
                if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 2;
                    direction = 'R';
                }
                else if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 2;
                    direction = 'U';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 0;
                    direction = 'U';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 3) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 0;
                    direction = 'R';
                }
                break; 
        }
    }
    else {
        switch (direction) {
            case 'L': 
                if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 0;
                    direction = 'R';
                }
                else if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 0;
                    direction = 'D';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 1;
                    direction = 'R';
                }
                else if (curr_cube_face_row_number == 3 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 1;
                    direction = 'D';
                }
                break; 
            case 'R': 
                if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 1;
                    direction = 'L';
                }
                else if (curr_cube_face_row_number == 1 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'U';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'L';
                }
                else if (curr_cube_face_row_number == 3 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 2;
                    new_cube_face_col_number = 1;
                    direction = 'U';
                }
                break; 
            case 'U': 
                if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 3;
                    new_cube_face_col_number = 0;
                    direction = 'R';
                }
                else if (curr_cube_face_row_number == 0  && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 3;
                    new_cube_face_col_number = 0;
                    direction = 'U';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 1;
                    direction = 'R';
                }
                break; 
            default: // case 'D' 
                if (curr_cube_face_row_number == 0 && curr_cube_face_col_number == 2) {
                    new_cube_face_row_number = 1;
                    new_cube_face_col_number = 1;
                    direction = 'L';
                }
                else if (curr_cube_face_row_number == 2 && curr_cube_face_col_number == 1) {
                    new_cube_face_row_number = 3;
                    new_cube_face_col_number = 0;
                    direction = 'L';
                }
                else if (curr_cube_face_row_number == 3 && curr_cube_face_col_number == 0) {
                    new_cube_face_row_number = 0;
                    new_cube_face_col_number = 2;
                    direction = 'D';
                }
                break; 
        }
    }

    // Check in which entry of the surface matrix we are positioned...
    auto row_in_cube = current_position.first % face_size; auto col_in_cube = current_position.second % face_size; 

    // ...and in which entry of the new cube we need to go 
    // The following is a helper index  
    auto flipping_idx = old_dir == 'L' ? (face_size - 1) - row_in_cube: 
                        old_dir == 'R' ? row_in_cube: 
                            old_dir == 'U' ? col_in_cube : (face_size - 1) - col_in_cube; 

    // Row and column in the new cube surface
    auto new_row_in_cube = direction == 'L' ? (face_size - 1) - flipping_idx: 
                            direction == 'R' ? flipping_idx: 
                                direction == 'U' ? (face_size - 1) : 0; 

    auto new_col_in_cube = direction == 'L' ? (face_size - 1) :
                            direction == 'R' ? 0 : 
                                direction == 'U' ? flipping_idx : (face_size - 1) - flipping_idx;

    // return the final coordinate
    return Coordinate((new_cube_face_row_number * face_size) + new_row_in_cube, (new_cube_face_col_number * face_size) + new_col_in_cube); 
}

// The algorithm that allows us to perform the steps in the map 
auto navigate_map(const std::vector<std::string>& grid, const std::vector<Instruction>& instructions, bool is_part_two = false, bool is_test = false) {

    auto wrapping_function = [&grid, &is_part_two, &is_test](const Coordinate& pos, char& dir) {
        if (!is_part_two) return wrap_grid(grid, pos, dir); 
        else return wrap_cube(grid, pos, dir, is_test); 
    }; 

    // Find the starting position 
    size_t starting_col = 0;
    while (grid.at(0).at(starting_col) != '.') {++starting_col; continue;} 
    Coordinate position(0, starting_col);  
    bool is_turning_instruction = false; 

    char direction = 'R'; // starting direction is right 

    // Go through the instructions
    for (const auto& ins: instructions) {
        // Change direction
        if (is_turning_instruction) {
            direction = change_direction(direction, ins.direction); 
            is_turning_instruction = false; 
            continue; 
        }

        // Move forward for ins.forward_steps steps
        for (size_t s = 0; s < ins.forward_steps; ++s) { 
            
            // If we are not at the edge, check the new tile onto which we stepped 
            if (!is_on_edge(grid, position, direction)) {
                auto new_position = move(position, direction);
                auto new_tile = grid.at(new_position.first).at(new_position.second); 
                
                if (new_tile == '#') {
                    break; 
                } 
                else if (new_tile == '.') position = new_position; 
                continue; 
            } 

            // Else, we're on the edge: we need to wrap the grid
            auto old_direction = direction; // save the old direction (useful for part 2)
            auto new_wrapped_position = wrapping_function(position, direction); 
            if (grid.at(new_wrapped_position.first).at(new_wrapped_position.second) == '#') {
                direction = old_direction; 
                break;
            } 
            position = new_wrapped_position; 
        }

        is_turning_instruction = true; 
    }

    // Finally, calculate the password
    int direction_score;
    switch (direction) {
        case 'R': 
            direction_score = 0; 
            break; 
        case 'D': 
            direction_score = 1; 
            break; 
        case 'L':
            direction_score = 2; 
            break; 
        default: // 'U'
            direction_score = 3; 
            break; 
    } 
    return 1000 * (position.first + 1) + 4 * (position.second + 1) + direction_score; 
}

void solve_part_one(const std::vector<std::string>& grid, const std::vector<Instruction>& instructions) {
    auto solution = navigate_map(grid, instructions);
    std::cout << "The solution to part one is " << solution << std::endl; 
}

void solve_part_two(const std::vector<std::string>& grid, const std::vector<Instruction>& instructions, bool is_test) {
    auto solution = navigate_map(grid, instructions, true, is_test);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


