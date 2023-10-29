#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <utility> 
#include <list> 
#include <algorithm>
#include <map>

struct Node {
    size_t row; 
    size_t col; 
    Node* parent = nullptr; 
    Node(size_t r, size_t c): row(r), col(c) {}  
    ~Node() {
        if (parent != nullptr) delete parent; 
    }
}; 

using Grid = std::vector<std::string>; 

auto remove_from_vector = [](std::vector<char>& vec, char val) {
    vec.erase(std::remove(vec.begin(), vec.end(), val), vec.end()); 
}; 

void get_data(const std::string&, Grid&); 
void solve_part_one(Grid& grid); 
void solve_part_two(Grid& grid); 


int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 12 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    Grid grid; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, grid); 

    if (is_test) {
        for (const auto& row: grid) {
            std::cout << row << std::endl; 
        }
    }

    solve_part_one(grid); 
    solve_part_two(grid); 

    return 0; 
}

void get_data(const std::string& file_name, Grid& grid) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            grid.push_back(line); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

} 

auto find_starting_points(Grid& grid, char starting_point_mark = 'S') {
    size_t n_rows = grid.size(); 
    std::vector<std::pair<size_t, size_t>> starting_points; 

    // Find the starting position
    size_t row{0}; 
    while (row < n_rows) {
        bool found = false; 
        size_t col{0}; 
        for (const auto& elevation: grid.at(row)) {
            if (elevation == starting_point_mark || starting_point_mark != 'S' && elevation == 'S') {
                std::pair<size_t, size_t> starting_point(row, col);
                starting_points.push_back(starting_point);  
                if (starting_point_mark == 'S') {
                    found = true; 
                    break; 
                } 
            }
            ++col; 
        }
        if (found) break; 
        ++row; 
    }
    return starting_points;  
}

// Approach: Breadth-First-Search
// Explore the new nodes level by level; at each level, add the new potential nodes to a 
// queue. Loop through the node and update their corresponding parents at each iterations. 
// When 'E' is reached, calculate how many steps the path took, and select the path wit the 
// smaller number of steps. Loop until the queue is emptied, having explored all possible paths. 
// Keep track of the explored nodes with a grid of booleans, and if a node was already 
// explored, skip the search through that node. 
auto explore_paths(Grid& grid, std::pair<size_t,size_t> starting_point) {
    size_t n_rows = grid.size(), n_cols = grid.at(0).size(); 
    auto row = starting_point.first;
    auto col = starting_point.second; 

    // This will keep track of the nodes explored in the grid: 
    std::vector<std::vector<bool>> explored_nodes(n_rows, std::vector<bool>(n_cols, false)); 
    size_t n_of_steps{0}; 
    n_of_steps--; // start from the largest size_t number  


    grid.at(row).at(col) = 'a'; // update the corresponding grid value so that it measures 'a' when exploring the paths
    Node* current_node = new Node(row, col); 
    auto start_row = row, start_col = col; 

    std::queue<Node*> q;
    q.push(current_node); 

    while (!q.empty()) {
        current_node = new Node(q.front()->row, q.front()->col);
        current_node->parent = q.front()->parent; 
        q.pop(); 

        // Check the possible directions in the grid that we can visit
        std::vector<char> directions{'u', 'd', 'l', 'r'}; 
        if (current_node->row == 0) remove_from_vector(directions, 'u');
        else if (current_node->row == n_rows - 1) remove_from_vector(directions, 'd'); 
        if (current_node->col == 0) remove_from_vector(directions, 'l'); 
        else if (current_node->col == n_cols - 1) remove_from_vector(directions, 'r'); 
        size_t n_possible_directions = directions.size(); 
        // Update rows and columns based on direction
        for (const auto& d: directions) {
            if (d == 'u') {
                row = current_node->row - 1; 
                col = current_node->col; 
            } else if (d == 'd') {
                row = current_node->row + 1; 
                col = current_node->col;
            } else if (d == 'l') {
                row = current_node->row;
                col = current_node->col - 1; 
            } else { // d == 'r
                row = current_node->row;
                col = current_node->col + 1; 
            } 
            // If the potential destination is too elevated or already explored, skip  
            if ((grid.at(row).at(col) > grid.at(current_node->row).at(current_node->col) + 1 &&
                grid.at(row).at(col) != 'E') || explored_nodes.at(row).at(col)) {
                    continue; 
            }
            // If the ptential destination is the arrival point, but we are in a too "low" node, skip;  
            if (grid.at(row).at(col) == 'E' && grid.at(current_node->row).at(current_node->col) != 'z') continue; 
            explored_nodes.at(row).at(col) = true;
            Node* new_node = new Node(row, col); 
            new_node->parent = current_node; 
            q.push(new_node); 

            // If we are at the final desintation, count the number of steps of the path 
            // And update the number of steps if the path is smaller  
            if (grid.at(new_node->row).at(new_node->col) == 'E') {
                Node* final_node = new_node; 
                final_node->parent = new_node->parent; 
                
                size_t tmp_n_of_steps{0}; 
                while (final_node->parent != nullptr) {
                    ++tmp_n_of_steps; 
                    final_node = final_node->parent; 
                } 
                if (tmp_n_of_steps < n_of_steps) {
                    n_of_steps = tmp_n_of_steps; 
                }
            }
        }
    }

    // Reset grid for part 2 
    grid.at(start_row).at(start_col) = 'S'; 
    return n_of_steps;  
}

auto find_size_best_path(Grid& grid, char starting_point) {
    auto starting_points = find_starting_points(grid, starting_point); 
    size_t n_steps_best_path{0}; 
    n_steps_best_path--; 
    for (const auto& s: starting_points) {
        size_t tmp_n_steps_best_path = explore_paths(grid, s); 
        if (tmp_n_steps_best_path < n_steps_best_path) n_steps_best_path = tmp_n_steps_best_path;
    }
    return n_steps_best_path; 
}

void solve_part_one(Grid& grid) {
    auto solution = find_size_best_path(grid, 'S'); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

void solve_part_two(Grid& grid) {
    auto solution = find_size_best_path(grid, 'a'); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}


