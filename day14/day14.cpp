#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <utility> 
#include <algorithm>
#include <unordered_set>
#include <stack>

// Approach: we will simulate a coordinate with a std::pair of integers. We will also define a RockPath struct able to store 
// a path of a rock according to the input data. The class (struct) also contains a function that checks if a 
// coordinates collides with the rock path. Such class will also calculate the extremes (as found in the input 
// data) of the map. To solve part one and part two, we will simply simulate the sand path one by one, and step by 
// step. This is done with a vanilla (brute force) algorithm for Part One, and with a Depth-First-Search optimization in Part II:
// in practice, at each step a stack data structure will keep track of the last visited coordinate before the sand comes to rest, 
// and the next cycle of the algorithm will restart from there rather than from the starting point. 

using Coordinate = std::pair<unsigned int, unsigned int>; 
const Coordinate STARTING_POINT = Coordinate(500, 0); 

// Helper functions (std::set's will store the coordinate as strings, which is useful in terms of speed) 
auto to_string(Coordinate c) {
    return std::to_string(c.first) + "-" + std::to_string(c.second); 
}; 

auto to_string(unsigned int x, unsigned int y) {
    return std::to_string(x) + "-" + std::to_string(y); 
}

// Helper function to convert a string back to Coordinate 
auto to_coordinate(std::string& c_string) {
    unsigned int first, second; 
    char tmp; 
    std::istringstream stream(c_string); 
    stream >> first >> tmp >> second; 
    return std::make_pair(first, second); 
}

struct RockPath {
    std::vector<Coordinate> paths; 
    unsigned int MIN_LEFT, MAX_RIGHT, MAX_TOP, MIN_BOTTOM; 

    auto add_coordinate(Coordinate c) {
        paths.push_back(c); 
    }

    // Check if a coordinate might collide with a rock 
    auto collides(Coordinate c) const {
        for (size_t i = 0; i < paths.size(); ++i) {
            if (i+1 == paths.size()) break;  
            auto path_first_c = paths.at(i); 
            auto path_second_c = paths.at(i+1); 
            // We have two cases: if x in the first path coordinate is equal to 
            // x in the second path coordinate, then we have a vertical path; if 
            // on the other hand if the two y's are equal, then we have a vertical path  
            // Vertical path
            if ((path_first_c.first == path_second_c.first) && (path_first_c.first == c.first)) {
                if (path_first_c.second <= path_second_c.second) {
                    // Upward path (numbers are increasing vertically)
                    if (c.second >= path_first_c.second && c.second <= path_second_c.second) return true; 
                } else {
                    // Downward path (numbers are increasing vertically)
                    if (c.second <= path_first_c.second && c.second >= path_second_c.second) return true;
                }
            }
            // Horizontal path
            else if ((path_first_c.second == path_second_c.second) && (path_first_c.second == c.second)) {
                if (path_first_c.first <= path_second_c.first) {
                    // Right-ward path 
                    if (c.first >= path_first_c.first && c.first <= path_second_c.first) return true; 
                } else {
                    // Left-ward path
                    if (c.first <= path_first_c.first && c.first >= path_second_c.first) return true;
                }
            }
        }
        return false; 
    }

    auto print() const {
        for (const auto& p: paths) {
            std::cout << "(" << p.first << "," << p.second << ")"; 
            if (p == paths.back()) std::cout << std::endl; 
            else std::cout << " -> "; 
        } 
    }

    auto set_min_left() {
        auto found_val = std::min_element(paths.begin(), paths.end(), [](Coordinate c1, Coordinate c2) {
            return c1.first < c2.first; 
        });
        this->MIN_LEFT = found_val->first; 
    }

    auto set_max_right() {
        auto found_val = std::max_element(paths.begin(), paths.end(), [](Coordinate c1, Coordinate c2) {
            return c1.first < c2.first; 
        });
        this->MAX_RIGHT = found_val->first; 
    }

    auto set_min_bottom() {
        auto found_val = std::max_element(paths.begin(), paths.end(), [](Coordinate c1, Coordinate c2) {
            return c1.second < c2.second; 
        });
        this->MIN_BOTTOM = found_val->second; 
    }

    auto set_max_top() {
        auto found_val = std::min_element(paths.begin(), paths.end(), [](Coordinate c1, Coordinate c2) {
            return c1.second < c2.second; 
        });
        this->MAX_TOP = found_val->second; 
    }

    auto set_extremes() {
        this->set_min_left(); 
        this->set_max_right(); 
        this->set_min_bottom(); 
        this->set_max_top(); 
    }

}; 

void get_data(const std::string&, std::vector<RockPath>&); 
void solve_part_one(const std::vector<RockPath>&); 
void solve_part_two(const std::vector<RockPath>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 14 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<RockPath> rock_paths; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, rock_paths); 

    if (is_test) {
        for (const auto& r: rock_paths) r.print(); 
    }

    solve_part_one(rock_paths); 
    solve_part_two(rock_paths); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<RockPath>& rock_paths) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        unsigned int tmp_number = 0;
        while (std::getline(input_file, line)) {
            RockPath rock_path; 
            Coordinate coordinate; 
            size_t counter = 0; 
            std::istringstream line_stream(line); 
            while (line_stream >> tmp_number) {
                auto is_new_coordinate = counter % 2 == 0; 
                is_new_coordinate ? coordinate.first = tmp_number : coordinate.second = tmp_number; 

                if (line_stream.peek() == ',') line_stream.ignore(); 

                if (line_stream.peek() == ' ') {
                    line_stream.ignore(); // ' ' 
                    line_stream.ignore(); // '-' 
                    line_stream.ignore(); // '>' 
                    line_stream.ignore(); // ' '
                }

                if (!is_new_coordinate) rock_path.add_coordinate(coordinate);  
                // else Coordinate coordinate; 
                ++counter; 
            }
            rock_path.set_extremes(); 
            rock_paths.push_back(rock_path); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
auto find_map_leftmost_point(const std::vector<RockPath>& r) {
    auto found_val = std::min_element(r.begin(), r.end(), [](RockPath r1, RockPath r2){
        return r1.MIN_LEFT < r2.MIN_LEFT;  
    });
    return found_val->MIN_LEFT; 
}

auto find_map_rightmost_point(const std::vector<RockPath>& r) {
    auto found_val = std::max_element(r.begin(), r.end(), [](RockPath r1, RockPath r2){
        return r1.MAX_RIGHT < r2.MAX_RIGHT;  
    });
    return found_val->MAX_RIGHT; 
}

auto find_map_bottom_point(const std::vector<RockPath>& r) {
    auto found_val = std::max_element(r.begin(), r.end(), [](RockPath r1, RockPath r2){
        return r1.MIN_BOTTOM < r2.MIN_BOTTOM;  
    });
    return found_val->MIN_BOTTOM; 
}

auto find_map_top_point(const std::vector<RockPath>& r) {
    auto found_val = std::min_element(r.begin(), r.end(), [](RockPath r1, RockPath r2){
        return r1.MAX_TOP < r2.MAX_TOP;  
    });
    return found_val->MAX_TOP; 
}

auto coordinate_collides_rock(const std::vector<RockPath>& r, Coordinate& c) {
    for (auto const& p: r) {
        if (p.collides(c)) return true; 
    }
    return false;
}

// Algorithm for part 1 
auto pour_sand(const std::vector<RockPath>& r) {

    // Find extreme points in the map 
    // const auto MAX_TOP = find_map_top_point(r); 
    const auto MAX_TOP = 0;
    const auto MIN_BOTTOM = find_map_bottom_point(r); 
    const auto LEFTMOST = find_map_leftmost_point(r); 
    const auto RIGHTMOST = find_map_rightmost_point(r); 

    // Keep track of the coordinates filled by sand and by air 
    std::unordered_set<std::string> sand_coordinates; 
    std::unordered_set<std::string> air_coordinates; 

    for (size_t i = LEFTMOST; i <= RIGHTMOST; ++i) {
        for (size_t j = MAX_TOP; j <= MIN_BOTTOM; ++j) {
            Coordinate c(i, j);
            if(coordinate_collides_rock(r, c)) continue;
            else air_coordinates.insert(to_string(i, j)); 
        }
    }

    size_t sand_counter = 0; 
    // Start pouring sand
    while (true) {
        Coordinate sand = STARTING_POINT;   // start pouring from the starting point   
        bool sand_at_rest = false; 
        
        while (!sand_at_rest) {
            // Go down vertically until a rock (or another sand) is encountered
            while(air_coordinates.find(to_string(sand)) != air_coordinates.end()) {
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                ++sand.second;
            }

            // Check if there are coordinates available in the down left or down right
            if (air_coordinates.find(to_string(sand.first-1, sand.second)) != air_coordinates.end() || 
                (sand.first - 1) < LEFTMOST) {
                --sand.first; 
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                continue;  
            } else if (air_coordinates.find(to_string(sand.first+1, sand.second)) != air_coordinates.end() || 
                (sand.first + 1) > RIGHTMOST) {
                ++sand.first;
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                continue; 
            }

            --sand.second; 
            auto air_tmp = air_coordinates.find(to_string(sand)); 
            air_coordinates.erase(air_tmp); 
            sand_coordinates.insert(to_string(sand)); 
            sand_at_rest = true; 
        }
        ++sand_counter; 
    }
}

void solve_part_one(const std::vector<RockPath>& r) {
    auto solution = pour_sand(r); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For the second part of the puzzle, as we have a potentially infinite map, 
// rather than keeping track of the coordinates containing air I will keep track of the 
// unavailable coordinates (either rock or sand). Rather than restarting from the origin
// every time, a std::stack keeps track of the new origin point every time a grain of sand comes
// to rest. This resembles a Depth-First-Search algorithm.   
auto pour_with_bottom(const std::vector<RockPath>& r) {

    // Find extreme points in the map 
    // const auto MAX_TOP = find_map_top_point(r); 
    const auto MAX_TOP = 0;
    const auto MIN_BOTTOM = find_map_bottom_point(r) + 2; 
    const auto LEFTMOST = find_map_leftmost_point(r); 
    const auto RIGHTMOST = find_map_rightmost_point(r); 

    // Keep track of the coordinates filled by sand and by air 
    std::unordered_set<std::string> unavailable_coordinates;  

    for (size_t i = LEFTMOST; i <= RIGHTMOST; ++i) {
        for (size_t j = MAX_TOP; j <= MIN_BOTTOM; ++j) {
            Coordinate c(i, j); 
            if(!coordinate_collides_rock(r, c)) continue;
            else unavailable_coordinates.insert(to_string(i,j)); 
        }
    }

    size_t sand_counter = 0; 
    std::stack<std::string> coming_from; 
    coming_from.push(to_string(STARTING_POINT)); 
    Coordinate sand, origin, tmp_starting_coordinate;
    // Start pouring sand

    while (true) {
        sand = to_coordinate(coming_from.top()); // start from the last stored grain of sand 
        origin = sand;
        bool sand_at_rest = false; 
        bool is_at_bottom = false; 
        int direction_from = 0; // 0: vertical; -1: top right; 1: top left 

        while (!sand_at_rest) {
            tmp_starting_coordinate = sand; 

            // Go down vertically until the bottom, or another sand grain or rock is encountered
            while(unavailable_coordinates.find(to_string(sand)) == unavailable_coordinates.end()) {
                if (sand.second == MIN_BOTTOM) {is_at_bottom = true; break;}  
                ++sand.second;
            }

            // If we went down at least by one step vertically, mark the direction as 0
            if (sand.first == tmp_starting_coordinate.first && sand.second > tmp_starting_coordinate.second+1) direction_from = 0; 

            // Check if sand can go down left or down right
            if (!is_at_bottom) {
                if (unavailable_coordinates.find(to_string(sand.first-1, sand.second)) == unavailable_coordinates.end()) {
                    --sand.first; 
                    direction_from = -1; 
                    continue;  
                }  
                if (unavailable_coordinates.find(to_string(sand.first+1, sand.second)) == unavailable_coordinates.end()) {
                    ++sand.first;
                    direction_from = 1; 
                    continue; 
                }
            }

            // For debugging - Print the map
            // std::cout << "SAND # " << sand_counter << std::endl;  
            // for (size_t j = MAX_TOP; j <= MIN_BOTTOM; ++j) {
            //     for (size_t i = LEFTMOST-10; i <= RIGHTMOST+10; ++i) {
            //         Coordinate c_tmp(i, j); 
            //         if (i == origin.first && j == origin.second && i == sand.first && j == sand.second) std::cout << "Y"; 
            //         else if (i == origin.first && j == origin.second) std::cout << "S"; 
            //         else if (j == MIN_BOTTOM) std::cout << "#";
            //         else if (unavailable_coordinates.find(to_string(c_tmp)) != unavailable_coordinates.end() && coordinate_collides_rock(r, c_tmp)) std::cout << "#";
            //         else if (unavailable_coordinates.find(to_string(c_tmp)) != unavailable_coordinates.end()) std::cout << "o";
            //         else std::cout << "."; 
            //     }
            //     std::cout << std::endl;  
            // }
            // std::cout << std::endl; 

            // The sand is now ready to rest - Update coordinates (we went down one extra step previously): 
            --sand.second; 
            // If the sand rests at the origin, we remove the origin from the stack; 
            if (sand.first == origin.first && sand.second == origin.second) coming_from.pop(); 
            else {
                // Update the origin - coming from the top
                if (direction_from == 0) {
                    if (unavailable_coordinates.find(to_string(sand.first, sand.second-1)) == unavailable_coordinates.end() &&
                        sand.first != origin.first || sand.second != origin.second); 
                        coming_from.push(to_string(sand.first, sand.second-1));
                }
                // Update the origin - coming from the top right
                else if (direction_from == -1) {
                    if (unavailable_coordinates.find(to_string(sand.first+1, sand.second+1)) == unavailable_coordinates.end() &&
                        origin.first > sand.first + 1) {
                        coming_from.push(to_string(sand.first+1, sand.second-1));
                    }
                }
                // Update the origin - coming from the top left
                else {
                    if (unavailable_coordinates.find(to_string(sand.first-1,sand.second+1)) == unavailable_coordinates.end()) {
                        // Only update the origin if we have moved two steps to the bottom right...
                        if (origin.first < sand.first - 1) {
                            coming_from.push(to_string(sand.first-1, sand.second-1)); 
                        } else {
                            // ...otherwise all the potential options have been visited and the origin can be removed
                            if (origin.first != STARTING_POINT.first && origin.second != STARTING_POINT.second)
                                coming_from.pop(); // no more coordinates to explore from bottom right, come back to previous starting point
                        }
                    }
                }
            }

            // If the sand rests at the starting point, return the sand counter 
            if (sand.first == STARTING_POINT.first && sand.second == STARTING_POINT.second) return ++sand_counter; 
            unavailable_coordinates.insert(to_string(sand)); 
            sand_at_rest = true; 
        }
        ++sand_counter; 
    }
    
}

void solve_part_two(const std::vector<RockPath>& r) {
    auto solution = pour_with_bottom(r); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}


