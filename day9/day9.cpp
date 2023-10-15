#include <string>
#include <sstream>
#include <fstream> 
#include <iostream>
#include <unordered_set> 
#include <vector> 
#include <utility> 

using steps = std::pair<char, int>; 

void get_data(const std::string&, std::vector<steps>&); 
void solve_part_one(const std::vector<steps>&); 
void solve_part_two(const std::vector<steps>&); 

int main(int argc, char* argv[]) {

    std::string file_name; 
    bool is_test; 
    std::vector<steps> instructions; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, instructions); 

    if (is_test) {
        for (const auto& i: instructions) {
            std::cout << i.first << " - " << i.second << std::endl; 
        }
    }

    solve_part_one(instructions); 
    solve_part_two(instructions); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<steps>& instructions) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            std::istringstream line_stream{line}; 
            char direction; 
            int n_steps; 
            line_stream >> direction; 
            line_stream >> n_steps; 
            instructions.emplace_back(direction, n_steps); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

} 

// This function checks where the tail and head are positioned, and moves the tail 
// accordingly. These are the positions to check: 
// if i_tail == i_head, it checks how far the head is horizontally 
// if j_tail == j_head, it checks how far the head is vertically 
// else it needs to make sure the diagonal distance is within one step max in either direction 
// The function returns true if the tail knot has moved
// For part two, the tail is the knot under examination, while the head is the knot preceding 
// the one under examination. 
auto check_and_move_tail(int& i_tail, int& j_tail,  const int i_head, const int j_head) -> bool {

    // Horizontal alignment
    if (i_head == i_tail) {
        if (j_head - j_tail > 1) {
            j_tail++; // We only neew to move one step as the function is called at every head's step
        } else if (j_head - j_tail < -1) {
            j_tail--; 
        }
        return true; 
    }

    // Vertical alignment 
    if (j_head == j_tail) {
        if (i_head - i_tail > 1) {
            i_tail++;
        } else if (i_head - i_tail < -1) {
            i_tail--; 
        }
        return true; 
    }

    if (std::abs(i_head - i_tail) > 1 || // The head is more than one step above/below the tail
        std::abs(j_head - j_tail) > 1) { // The head is more than one step left/right of the tail
        i_tail = (i_head > i_tail) ? i_tail+1 : i_tail-1; // We only neew to move one step as the 
        j_tail = (j_head > j_tail) ? j_tail+1 : j_tail-1; // function is called at every head's step
        return true; 
    } 

    return false; 

}

// Approach: loop through the instructions with the head (starting from 0,0) and 
// have the tail move accordingly with the check_and_move tail function defined above.
// Insert in a std::unordered_set (a hash map) the steps by the tail, 
// and finally return the size of such set 
auto count_visited_positions(const std::vector<steps>& instructions) {
    std::unordered_set<std::string> visited_positions; 
    int i_head{0}, i_tail{0}, j_head{0}, j_tail{0}; // set starting positions
    // For simplicity we assume the starting point is (0,0) -> the grid can take negative values, 
    // potentially, but this is not relevant for the solution of the problem 

    for (const auto& i: instructions) {
        auto direction = i.first;
        auto n_steps = i.second; 

        while (n_steps > 0) {
            switch (direction) {
                case 'U': 
                    i_head++; 
                    break; 
                case 'D': 
                    i_head--; 
                    break; 
                case 'R':
                    j_head++; 
                    break; 
                case 'L':
                    j_head--;
                    break;
                default:
                    break; 
            }
            if (check_and_move_tail(i_tail, j_tail,  i_head, j_head)) {
                // Note that .insert() only inserts if the key is not already in the set 
                visited_positions.insert(std::to_string(i_tail) + "-" + std::to_string(j_tail));
            } 
            n_steps--;
        } 
    }
    return visited_positions.size(); 
}

// Function for part 2. The logic is the same as with part 1, but we are using 
// a vector of knots instead of just the head-tail knots. 
auto count_visited_positions_large_rope(const std::vector<steps>& instructions) {
    std::unordered_set<std::string> visited_positions;  
    // Create a rope of ten knots all at the starting point (0,0)
    std::vector<std::pair<int, int>> rope(10, std::make_pair(0,0)); 

    for (const auto& i: instructions) {
        auto direction = i.first;
        auto n_steps = i.second; 

        while (n_steps > 0) {
            // Start with the head knot:
            switch (direction) {
                case 'U': 
                    rope.at(0).first++; 
                    break; 
                case 'D': 
                    rope.at(0).first--; 
                    break; 
                case 'R':
                    rope.at(0).second++; 
                    break; 
                case 'L':
                    rope.at(0).second--;
                    break;
                default:
                    break; 
            }
            // Now loop through the other rope's knots and update the knots positions
            // This is optimized by breaking the loop when a knot stops moving 
            // (we don't need to check the next knots)
            for (size_t k = 1; k < rope.size(); ++k) {
                if(!check_and_move_tail(rope.at(k).first, rope.at(k).second,  
                    rope.at(k-1).first, rope.at(k-1).second)) {
                        break; 
                    }

                if (k == rope.size() - 1) {
                    // Note that .insert() only inserts if the key is not already in the set 
                    visited_positions.insert(std::to_string(rope.at(9).first) + "-" + std::to_string(rope.at(9).second)); 
                }
            }
            
            n_steps--;
        } 
    }
    return visited_positions.size(); 
}


void solve_part_one(const std::vector<steps>& instructions) {
    auto solution = count_visited_positions(instructions); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}
void solve_part_two(const std::vector<steps>& instructions) {
    auto solution = count_visited_positions_large_rope(instructions); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}