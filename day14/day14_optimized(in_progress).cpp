#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <utility> 
#include <algorithm>
#include <unordered_set>
#include <stack>

using Coordinate = std::pair<unsigned int, unsigned int>; 
const Coordinate STARTING_POINT = Coordinate(500, 0); 

struct CoordinateHash {
    std::size_t operator () (const Coordinate& c) const {
        auto h1 = std::hash<unsigned int>{}(c.first);
        auto h2 = std::hash<unsigned int>{}(c.second);
        return h1 ^ h2;
    }
};

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

auto pour_sand(const std::vector<RockPath>& r) {

    // Find extreme points in the map 
    // const auto MAX_TOP = find_map_top_point(r); 
    const auto MAX_TOP = 0;
    const auto MIN_BOTTOM = find_map_bottom_point(r); 
    const auto LEFTMOST = find_map_leftmost_point(r); 
    const auto RIGHTMOST = find_map_rightmost_point(r); 

    // Keep track of the coordinates filled by sand and by air 
    std::unordered_set<Coordinate, CoordinateHash> sand_coordinates; 
    std::unordered_set<Coordinate, CoordinateHash> air_coordinates; 

    for (size_t i = LEFTMOST; i <= RIGHTMOST; ++i) {
        for (size_t j = MAX_TOP; j <= MIN_BOTTOM; ++j) {
            Coordinate c(i, j);
            bool is_rock = false; 
            if(coordinate_collides_rock(r, c)) continue;
            else air_coordinates.insert(c); 
        }
    }

    size_t sand_counter = 0; 
    bool bottom_reached = false;

    // Start pouring sand
    while (true) {
        Coordinate sand(STARTING_POINT.first, STARTING_POINT.second);   // start pouring in the first potential space  
        bool sand_at_rest = false; 
        
        while (!sand_at_rest) {
            while(air_coordinates.find(sand) != air_coordinates.end()) {
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                ++sand.second;
            }

            if (air_coordinates.find(Coordinate(sand.first-1, sand.second)) != air_coordinates.end() || 
                (sand.first - 1) < LEFTMOST) {
                --sand.first; 
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                continue;  
            } else if (air_coordinates.find(Coordinate(sand.first+1, sand.second)) != air_coordinates.end() || 
                (sand.first + 1) > RIGHTMOST) {
                ++sand.first;
                if (sand.second == MIN_BOTTOM) return sand_counter; 
                continue; 
            }

            --sand.second; 
            auto air_tmp = air_coordinates.find(sand); 
            air_coordinates.erase(air_tmp); 
            sand_coordinates.insert(sand); 
            sand_at_rest = true; 
        }
        ++sand_counter; 
    }
}

void solve_part_one(const std::vector<RockPath>& r) {
    auto solution = pour_sand(r); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// Attempt: part two with Depth-First-Search
auto pour_with_bottom(const std::vector<RockPath>& r) {
    // Find extreme points in the map 
    // const auto MAX_TOP = find_map_top_point(r); 
    const auto MAX_TOP = 0;
    const auto MIN_BOTTOM = find_map_bottom_point(r) + 2; 
    const auto LEFTMOST = find_map_leftmost_point(r); 
    const auto RIGHTMOST = find_map_rightmost_point(r); 
    // Keep track of the coordinates filled by sand and by air 
    std::unordered_set<Coordinate, CoordinateHash> unavailable_coordinates;  
    for (size_t i = LEFTMOST; i <= RIGHTMOST; ++i) {
        for (size_t j = MAX_TOP; j <= MIN_BOTTOM; ++j) {
            Coordinate c(i, j);
            bool is_rock = false; 
            if(!coordinate_collides_rock(r, c)) continue;
            else unavailable_coordinates.insert(c); 
        }
    }
    size_t sand_counter = 0; 
    bool is_start = true; 
    std::stack<Coordinate> coming_from; 
    coming_from.push(STARTING_POINT); 
    // Start pouring sand
    while (true) {
        Coordinate sand = coming_from.top(); 
        Coordinate origin = sand;
        auto current_first = sand.first;
        bool sand_at_rest = false; 
        bool is_at_bottom = false; 
        int direction_from = 0; 
        // std::cout << "starting from: " << "(" << sand.first << "," << sand.second << ")" << std::endl; 
        while (!sand_at_rest) {
            auto tmp_starting_coorinate = sand; 
            while(unavailable_coordinates.find(sand) == unavailable_coordinates.end()) {
                if (sand.second == MIN_BOTTOM) {is_at_bottom = true; break;}  
                ++sand.second;
            }

            if (sand.first == tmp_starting_coorinate.first && sand.second > tmp_starting_coorinate.second+1) direction_from = 0; 
 
            if (!is_at_bottom) {
                Coordinate tmp(sand.first - 1, sand.second); 
                if (unavailable_coordinates.find(tmp) == unavailable_coordinates.end()) {
                    --sand.first; 
                    direction_from = -1; 
                    continue;  
                }  
                tmp.first = sand.first + 1; 
                if (unavailable_coordinates.find(tmp) == unavailable_coordinates.end()) {
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
            //         else if (unavailable_coordinates.find(c_tmp) != unavailable_coordinates.end() && coordinate_collides_rock(r, c_tmp)) std::cout << "#";
            //         else if (unavailable_coordinates.find(c_tmp) != unavailable_coordinates.end()) std::cout << "o";
            //         else std::cout << "."; 
            //     }
            //     std::cout << std::endl;  
            // }
            // std::cout << std::endl; 

            --sand.second; 
            // std::cout << "new sand: " << "(" << sand.first << "," << sand.second << ")" << std::endl; 
            if (sand.first == origin.first && sand.second == origin.second) coming_from.pop(); 
            else {
                if (direction_from == 0) {
                    Coordinate new_c(sand.first, sand.second-1); 
                    if (unavailable_coordinates.find(new_c) == unavailable_coordinates.end() &&
                        sand.first != coming_from.top().first || sand.second != coming_from.top().second); 
                        coming_from.push(Coordinate(sand.first, sand.second-1));
                    // std::cout << "Adding: " << "(" << sand.first << "," << sand.second-1 << ")" << std::endl;
                }
                else if (direction_from == -1) {
                    Coordinate new_c(sand.first+1, sand.second+1); 
                    if (unavailable_coordinates.find(new_c) == unavailable_coordinates.end() &&
                        coming_from.top().first > sand.first + 1) {
                        coming_from.push(Coordinate(sand.first+1, sand.second-1));
                        // std::cout << "Adding: " << "(" << sand.first+1 << "," << sand.second-1 << ")" << std::endl;
                    }
                }
                else {
                    // coming_from.pop(); 
                    // coming_from.push(Coordinate(sand.first-1, sand.second-1));
                    Coordinate new_c(sand.first-1, sand.second-1); 
                    if (unavailable_coordinates.find(new_c) == unavailable_coordinates.end()) {
                        if (coming_from.top().first < sand.first - 1) {
                            coming_from.push(Coordinate(sand.first-1, sand.second-1)); 
                        } else {
                            if (coming_from.top().first != STARTING_POINT.first && coming_from.top().second != STARTING_POINT.second)
                            coming_from.pop(); // no more coordinates to explore from bottom right, come back to previous starting point
                        }
                        // std::cout << "Adding: " << "(" << sand.first-1 << "," << sand.second-1 << ")" << std::endl;
                    }
                }
            }

            // std::cout << "goes to " << sand.first << "-" << sand.second << std::endl;
            if (sand.first == STARTING_POINT.first && sand.second == STARTING_POINT.second) return ++sand_counter; 
            unavailable_coordinates.insert(sand); 
            sand_at_rest = true; 
        }
        ++sand_counter; 
    }
    
}

void solve_part_two(const std::vector<RockPath>& r) {
    auto solution = pour_with_bottom(r); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}


