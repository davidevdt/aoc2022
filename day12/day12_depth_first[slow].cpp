// #include <iostream> 
// #include <string>
// #include <sstream>
// #include <fstream> 
// #include <vector> 
// #include <utility> 
// #include <list> 
// #include <algorithm>

// // Similarly to day 8, I will treat the grid of heights as a 
// // vector of strings. The coordinates will be stored
// // as std::pairs. 
// // Furthermore,  I will exploit Unicode character representation that allows for 
// // char arithmetic: for example 'b' - 'a' = 1 
// using Coordinate = std::pair<size_t, size_t>;  
// using Grid = std::vector<std::string>; 
// using Paths = std::vector<std::list<Coordinate>>; // this vector will collect all the explored paths across the grid

// auto remove_from_vector = [](std::vector<char>& vec, char val) {
//     vec.erase(std::remove(vec.begin(), vec.end(), val), vec.end()); 
// }; 

// void get_data(const std::string&, Grid&); 
// void solve_part_one(Grid& grid); 
// // void solve_part_two(std::vector<Monkey>); 


// int main(int argc, char* argv[]) {

//     std::cout << ">>> Advent Of Code 2022 - Day 12 <<<" << std::endl; 

//     std::string file_name; 
//     bool is_test; 
//     Grid grid; 

//     if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
//     if (is_test) file_name = "./test.txt"; 
//     else file_name = "./input.txt"; 

//     get_data(file_name, grid); 

//     if (is_test) {
//         for (const auto& row: grid) {
//             std::cout << row << std::endl; 
//         }
//     }

//     solve_part_one(grid); 
//     // solve_part_two(); 

//     return 0; 
// }

// void get_data(const std::string& file_name, Grid& grid) {
//     std::ifstream input_file{file_name}; 

//     if (input_file.is_open()) {
//         std::string line; 
//         while (std::getline(input_file, line)) {
//             grid.push_back(line); 
//         }
//     } else {
//         std::cerr << "Could not open the file." << std::endl; 
//         exit(0); 
//     }

//     input_file.close(); 

// } 

// auto explore_paths(size_t row, size_t col, const Grid& grid, std::list<Coordinate> path, 
//     const char direction_from, const size_t& n_rows, const size_t& n_cols) {

//     Coordinate current_coordinate{row, col}; 
//     path.push_back(current_coordinate); 

//     Paths paths; 
//     if (grid.at(row).at(col) == 'E') {
//         paths.push_back(path);
//         return paths; 
//     }
    
//     // Check the possible directions in the grid that we can visit
//     std::vector<char> directions{'u', 'd', 'l', 'r'}; 
//     if (row == 0) remove_from_vector(directions, 'u');
//     else if (row == n_rows - 1) remove_from_vector(directions, 'd'); 
//     if (col == 0) remove_from_vector(directions, 'l'); 
//     else if (col == n_cols - 1) remove_from_vector(directions, 'r'); 
//     remove_from_vector(directions, direction_from); 
//     auto n_possible_directions = directions.size(); 
//     if (n_possible_directions == 0) return paths; 

//     for (const auto& d: directions) {
//         char direction_from;
//         if (d == 'u') {
//             row = current_coordinate.first - 1; 
//             col = current_coordinate.second; 
//             direction_from = 'd'; 
//         } else if (d == 'd') {
//             row = current_coordinate.first + 1; 
//             col = current_coordinate.second;
//             direction_from = 'u'; 
//         } else if (d == 'l') {
//             row = current_coordinate.first;
//             col = current_coordinate.second - 1; 
//             direction_from = 'r'; 
//         } else { // d == 'r
//             row = current_coordinate.first;
//             col = current_coordinate.second + 1; 
//             direction_from = 'l'; 
//         }     

//         // Don't visit if: (a) the node has already been visited in the current path; 
//         // (b) the next step is too elevated AND (c) the destination is not 'E'
//         Coordinate destination_coordinate = Coordinate(row, col); 
//         auto condition_a = std::find(path.begin(), path.end(), destination_coordinate) != path.end(); 
//         auto condition_b = grid.at(row).at(col) > grid.at(current_coordinate.first).at(current_coordinate.second) + 1; 
//         auto condition_c = grid.at(row).at(col) != 'E'; 
//         if (condition_a || (condition_b && condition_c)) {
//             --n_possible_directions; 
//             continue;  
//         }
//         // If the destination 'E' is not the only direction left, 
//         // then continue through the path
//         if (grid.at(row).at(col) == 'E' && n_possible_directions > 1) continue; 
//         // Explore new paths in the given direction
//         auto new_paths = explore_paths(row, col, grid, path, direction_from, n_rows, n_cols);
//         if (new_paths.size() > 0) paths.insert(paths.end(), new_paths.begin(), new_paths.end());  
//     }
//     return paths; 
// }

// auto explore_paths(Grid& grid) {

//     size_t n_rows = grid.size(), n_cols = grid.at(0).size(); 

//     // Find the starting position
//     size_t row{0}, col; 
//     while (true) {
//         bool found = false; 
//         size_t col_tmp{0}; 
//         for (const auto& height: grid.at(row)) {
//             if (height == 'S') {
//                 col = col_tmp;
//                 found = true; 
//                 break; 
//             }
//             ++col_tmp; 
//         }
//         if (found) break; 
//         ++row; 
//     }

//     grid.at(row).at(col) = 'a'; // update the corresponding grid value so that it measures 'a' when exploring the paths
//     Coordinate current_coordinate = Coordinate(row, col); 
//     auto start_row = row, start_col = col; 
    
//     // Check the possible directions in the grid that we can visit
//     std::vector<char> directions{'u', 'd', 'l', 'r'}; 
//     if (row == 0) remove_from_vector(directions, 'u');
//     else if (row == n_rows - 1) remove_from_vector(directions, 'd'); 
//     if (col == 0) remove_from_vector(directions, 'l'); 
//     else if (col == n_cols - 1) remove_from_vector(directions, 'r'); 

//     // Explore all possible paths across the various coordinates of the grid
//     // by calling the overloaded explore_paths function 
//     Paths paths; 
//     std::list<Coordinate> path;  
//     path.push_back(current_coordinate); 
//     for (const auto& d: directions) {
//         char direction_from;
//         if (d == 'u') {
//             row = current_coordinate.first - 1; 
//             col = current_coordinate.second; 
//             direction_from = 'd'; 
//         } else if (d == 'd') {
//             row = current_coordinate.first + 1; 
//             col = current_coordinate.second;
//             direction_from = 'u'; 
//         } else if (d == 'l') {
//             row = current_coordinate.first;
//             col = current_coordinate.second - 1; 
//             direction_from = 'r'; 
//         } else { // d == 'r
//             row = current_coordinate.first;
//             col = current_coordinate.second + 1; 
//             direction_from = 'l'; 
//         }   

//         // Don't visit if: (a) the node has already been visited in the current path; 
//         // (b) the next step is too elevated AND (c) the destination is not 'E'
//         // (d) we have the destination 'E' among the options but it is not the only one left 
//         Coordinate destination_coordinate = Coordinate(row, col); 
//         auto condition_a = std::find(path.begin(), path.end(), destination_coordinate) != path.end(); 
//         auto condition_b = grid.at(row).at(col) > grid.at(current_coordinate.first).at(current_coordinate.second) + 1; 
//         auto condition_c = grid.at(row).at(col) != 'E'; 
//         auto condition_d = grid.at(row).at(col) == 'E' && directions.size() > 1; 
//         if (condition_a || (condition_b && condition_c)) continue;  
//         // Explore new paths in the given direction
//         auto new_paths = explore_paths(row, col, grid, path, direction_from, n_rows, n_cols);
//         if (new_paths.size() > 0) paths.insert(paths.end(), new_paths.begin(), new_paths.end()); 

//     }

//     grid.at(start_row).at(start_col) = 'S'; // reset the starting grid value 
//     return paths; 
// }

// auto find_shortest_path(Grid& grid) {
//     auto paths = explore_paths(grid); 
//     auto shortest_path = std::min_element(
//         paths.begin(), paths.end(), [](const std::list<Coordinate>& a, std::list<Coordinate>& b) {
//             return a.size() < b.size(); 
//         }
//     ); 
//     // Use this for debug: 
//     // for (auto& c: *shortest_path) std::cout << c.first << " - " << c.second << std::endl; 
//     return shortest_path->size() - 1; // -1 because we are counting the steps, not the actual number of coordinates explored 
// }

// void solve_part_one(Grid& grid) {
//     auto solution = find_shortest_path(grid); 
//     std::cout << "The solution to part one is " << solution << std::endl; 
// }

// // // For part two we need to remove the worry level and increase the rounds to 10000
// // void solve_part_two(std::vector<Monkey> monkeys) {
// //     auto solution = play_keep_away(monkeys, 0, 10000); 
// //     std::cout << "The solution to part two is " << solution << std::endl; 
// // }


