#include <iostream> 
#include <vector>
#include <string> 
#include <fstream>

/* For this assignment, the tree grid will be stored as a matrix. 
In particular, it will be a vector of containers, or, to say it better,
as a vector of strings (we will consider strings as vectors of chars). */

void get_data(const std::string&, std::vector<std::string>&);
void solve_part_one(const std::vector<std::string>&); 
void solve_part_two(const std::vector<std::string>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 8 <<<" << std::endl; 

    bool is_test = false; 
    std::string file_name{};
    std::vector<std::string> tree_grid; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 

    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, tree_grid); 

    if (is_test) {
        std::cout << tree_grid.size()  << " by " << tree_grid.at(0).size() << "grid. Elements:" << std::endl;  
        for (const auto& t: tree_grid) std::cout << t << std::endl; 
    }

    solve_part_one(tree_grid); 
    solve_part_two(tree_grid); 

}

void get_data(const std::string& file_name, std::vector<std::string>& tree_grid) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string grid_line; 

        while (std::getline(input_file, grid_line)) {
            tree_grid.push_back(grid_line);     
        }

    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 
}

// The function takes the reference tree size as input, along with the position i-j of the tree grid
// that such reference size must be compared with. If the current tree is larger, then 
// keep looking to the direction given as input (up-down-left-right), and return if we reached the
// edge of the grid. Otherwise, return false. 
auto compare_trees(const std::vector<std::string>& tree_grid, const char& tree_size, size_t i, size_t j, const char direction) {

    auto is_visible = tree_size > tree_grid.at(i).at(j); 
    if (is_visible) {
        // base case: if the tree is visible w.r.t the external trees, then it is a visible tree
        if (i == 0 || j == 0 || i == tree_grid.size() - 1 || j == tree_grid.at(0).size() - 1) return true; 

        switch (direction) {
            case 'u': // keep looking up 
                return is_visible && compare_trees(tree_grid, tree_size, i - 1, j, direction); 
            case 'd': // keep looking down
                return is_visible && compare_trees(tree_grid, tree_size, i + 1, j, direction);
            case 'l': // keep looking left
                return is_visible && compare_trees(tree_grid, tree_size, i, j - 1, direction);
            case 'r': // keep looking right
                return is_visible && compare_trees(tree_grid, tree_size, i, j + 1, direction);
            default: 
                return is_visible; 
        }
    }

    // tree is not visible
    return false; 

}

// Approach: loops across all the trees internal to the grid (excluding the edge) 
// and check whether they are visible from any of the directions. If that's the case, 
// increase the count of visible trees by one as soon as one tree is considered visible. 
// This could be optimized by taking into account the i-j position in the grid: if we are
// close to a specific edge, we could reshuffle the order in which the search (that is, the
// order of the calls to the compare_trees functions) is performed. 
auto count_visible_trees(const std::vector<std::string>& tree_grid) {
    auto n_rows = tree_grid.size(); 
    auto n_cols = tree_grid.at(0).size(); 
    auto n_visible_trees = n_rows * 2 + n_cols * 2 - 4; // the external trees are visible
    for (size_t i = 1; i <= n_rows - 2; ++i) {
        for (size_t j = 1; j <= n_cols - 2; ++j) {
            const char tree_size = tree_grid.at(i).at(j); 
            n_visible_trees += \
                (compare_trees(tree_grid, tree_size, i - 1, j, 'u')) || // look up
                (compare_trees(tree_grid, tree_size, i + 1, j, 'd')) || // or look down
                (compare_trees(tree_grid, tree_size, i, j - 1, 'l')) || // or look left
                (compare_trees(tree_grid, tree_size, i, j + 1, 'r'));   // or look right
        }
    }
    return n_visible_trees; 
}

/* Alternative idea: loop throught the rows and columns of the grid from all for directions. 
These are the loops the we should implement: 
    * row-wise from left 
    * row-wise from right
    * column-wise from up
    * column-wise from down
Across the loop, keep track of the largest tree. Whenever a new maximum is found, 
add it to a std::unordered_set that keeps track of the visible trees. If the position i-j
is already in the set, go to the next loop iteration. If the found tree has height 9, break the
inner loop. 
When the outer loop has ended, add the number of trees in the edges to the number of elements 
inserted in the set, and return. 
*/

void solve_part_one(const std::vector<std::string>& tree_grid) {
    auto solution = count_visible_trees(tree_grid); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For part 2: 
// The function calculates the scenic score of a specific tree, by calculating the 
// corresponding scores in each directions and mulitplying such scores with each other. 
// It takes as input the corresponding positions i_ref-j_ref of the tree under examination.
auto calculate_tree_scenic_score(const std::vector<std::string>& tree_grid, const size_t i_ref, const size_t j_ref) {
    const char tree_size = tree_grid.at(i_ref).at(j_ref); 
    size_t scenic_score = 1; 

    // Score up
    size_t tmp_score = 1; // note that this already includes either the "edge score" or the "high next tree" score
    int i = i_ref - 1; 
    while ( i > 0 && tree_grid.at(i).at(j_ref) < tree_size) {
        tmp_score += 1; 
        --i; 
    }
    scenic_score *= tmp_score; 

    // Score down
    tmp_score = 1; 
    i = i_ref  + 1;
    while ( i < tree_grid.size() - 1 && tree_grid.at(i).at(j_ref) < tree_size) {
        tmp_score += 1; 
        ++i; 
    }
    scenic_score *= tmp_score; 

    // Score left
    tmp_score = 1; 
    int j = j_ref - 1;
    while ( j > 0 && tree_grid.at(i_ref).at(j) < tree_size) {
        tmp_score += 1; 
        --j; 
    }
    scenic_score *= tmp_score; 

    // Score right
    tmp_score = 1; 
    j = j_ref + 1;
    while ( j < tree_grid.at(0).size() - 1 && tree_grid.at(i_ref).at(j) < tree_size) {
        tmp_score += 1; 
        ++j; 
    }
    scenic_score *= tmp_score; 

    return scenic_score;
}

// Approach: loops through every tree in the grid, and calculate the scenic score
// for each tree. Return the maximum scenic score. 
auto find_best_scenic_score(const std::vector<std::string>& tree_grid) {
    size_t scenic_score = 0; 
    size_t tmp_score; 

    for (size_t i = 1; i < tree_grid.size() - 1; ++i) {
        for (size_t j = 1; j < tree_grid.at(0).size() - 1; ++j) {
            if ((tmp_score = calculate_tree_scenic_score(tree_grid, i, j)) > scenic_score) {
                scenic_score = tmp_score;
            }
        }
    }
    return scenic_score; 
}

void solve_part_two(const std::vector<std::string>& tree_grid) {
    auto solution = find_best_scenic_score(tree_grid); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}