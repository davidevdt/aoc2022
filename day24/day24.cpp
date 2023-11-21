#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <set>
#include <cmath>

/* This is a faster implementation w.r.t. the original one that can be found in the original version of the day 
 * (included in this folder). As proposed in the introduction of that file, instead of checking all the potential 
 * blizzard moves across the cyclic time span, we keep the blizzards fixed in their original positions. What I'll do is 
 * to check if the generated neighbors at each round of the shortest path algorithm can potentially clash with the blizzards
 * by moving the new position into the opposite direction of the blizzard, for a number of step equal to the time spent so far.
 * If in that position there is a blizzard going to a specific direction, then there is a clash (at the current time step)
 * with the blizzard and the neighbor is not proposed to the algorithm. This leads to a substantial decrease in the 
 * algorithm's running time. 
*/ 

 // Useful to calculate the modulo between two numbers (in case of negative modulo, it still returns a positive number)
 // This is used when we need to check if we are going to match a blizzard when moving backwards in time 
 size_t stable_modulo(int a, int b) {
  return ((a % b) + b) % b;
}

enum class Dir {
  N, E, S, W, NONE // NONE: stand still in the same position 
};

// Stores the coordinates and helper methods 
struct Coordinate {
  
  struct Hash {
    size_t operator()(const Coordinate& c) const {
      auto hash_row = std::hash<size_t>()(c.row); 
      auto hash_col = std::hash<size_t>()(c.col); 
      return hash_row ^ hash_col;
    }
  };
  
  size_t row; size_t col; size_t dist_from_target; 
  
  explicit Coordinate(size_t r, size_t c) noexcept: row(r), col(c), dist_from_target(0) {}
  explicit Coordinate(size_t r, size_t c, const Coordinate& target) noexcept: row(r), col(c) {
    this->set_distance(target); 
  }
  Coordinate() : row(0), col(0), dist_from_target(0) {}
  Coordinate(const Coordinate& other): row(other.row), col(other.col), dist_from_target(0) {}
  
  // Set the distance of the coordinate from the target (useful for the Dijkstra heuristics)
  auto set_distance(const Coordinate& target) -> size_t {
    this->dist_from_target = std::abs(this->row - target.row) + std::abs(this->col - target.col);
  }
  
  // Get all the neighbors of a coordinate
  // This is the new part compared to the originally proposed solution: instead of proposing all possible neighbors, 
  // I will check that the proposed neighbors are visitable: 
  // (a) are they the starting or the arrival point? 
  // (b) are they inside the map instead? If so, do they clash with a blizzard? To make sure of it, go backwards in time 
  // and check if a blizzard was there in the original position
  auto get_neighbors(size_t n_rows, size_t n_cols, const std::vector<std::string>& map, int time) const -> std::vector<Coordinate> {
    std::vector<Coordinate> neighbors; 
    size_t internal_rows = n_rows - 2; 
    size_t internal_cols = n_cols - 2;

    for (Dir d: std::vector<Dir>({Dir::N, Dir::E, Dir::S, Dir::W, Dir::NONE})) {
      
      auto new_neighbor = this->get_neighbor(d);

      if ((new_neighbor.row == 0 && new_neighbor.col == 1) || 
          (new_neighbor.row == n_rows-1 && new_neighbor.col == n_cols-2) || (
            (new_neighbor.row > 0 && new_neighbor.row < n_rows-1 && 
              new_neighbor.col > 0 && new_neighbor.col < n_cols-1) &&
                map.at(stable_modulo(new_neighbor.row + time - 1, internal_rows)+1).at(new_neighbor.col) != '^' &&
                map.at(new_neighbor.row).at(stable_modulo(new_neighbor.col - time - 1, internal_cols) + 1) != '>' &&
                map.at(stable_modulo(new_neighbor.row - time - 1, internal_rows) + 1).at(new_neighbor.col) != 'v' &&
                map.at(new_neighbor.row).at(stable_modulo(new_neighbor.col + time - 1, internal_cols) + 1) != '<')
      ) neighbors.push_back(new_neighbor);   
    }
    
    return neighbors; 
  }
  
  bool operator==(const Coordinate& other) const {
    return this->row == other.row && this->col == other.col; 
  }
  
  // Get neighboring coordinate in a specific direction 
  auto get_neighbor(Dir d) const -> Coordinate{
    Coordinate c(*this);
    switch(d) {
    case Dir::N:
      --c.row; break; 
    case Dir::S: 
      ++c.row; break; 
    case Dir::E:
      ++c.col; break; 
    case Dir::W:
      --c.col; break; 
    default: return *this; 
    }
    return c; 
  }
  
  auto to_string() const -> std::string {
    return std::to_string(row) + '-' + std::to_string(col); 
  } 
};

// Struct that stores blizzards' infromation and methods (useful for using the blizzards as keys 
// in unordered sets and maps)
struct Blizzard {
  
  struct Hash {
    size_t operator()(const Blizzard& b) const {
      auto hash_pos = Coordinate::Hash()(b.pos); 
      auto hash_dir = std::hash<size_t>()(static_cast<size_t>(b.d)); 
      return hash_pos ^ hash_dir;
    }
  };
  
  bool operator==(const Blizzard& other) const {
    return this->pos == other.pos && this->d == other.d; 
  }
  

  Coordinate pos; 
  Dir d; 
    
  explicit Blizzard(Coordinate p_, Dir d_) noexcept: pos(p_), d(d_) {}
};

void get_data(const std::string&, std::vector<std::string>&);  
void solve_part_one(const std::vector<std::string>&); 
void solve_part_two(const std::vector<std::string>&);

int main(int argc, char* argv[]) {
  
  std::cout << ">>> Advent Of Code 2022 - Day 24 <<<" << std::endl; 
  
  std::string file_name; 
  bool is_test = false; 
  std::vector<std::string> map; 
  
  if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
  if (is_test) file_name = "./test.txt"; 
  else file_name = "./input.txt"; 
  
  get_data(file_name, map); 
  
  if (is_test) {
    for (const auto& map_line: map) std::cout << map_line << std::endl; 
  }
  
  solve_part_one(map);
  solve_part_two(map); 
      
  return 0; 
}


// Get the input data
void get_data(const std::string& file_name, std::vector<std::string>& map) {
  
  std::ifstream input_file{file_name}; 
  
  if (input_file.is_open()) {
    std::string line; 
    long int row_counter = 0; 
    while (std::getline(input_file, line)) {
      map.push_back(line); 
    }
  } else {
    std::cerr << "Could not open the file." << std::endl; 
    exit(0); 
  }
  input_file.close(); 
} 

// Algorithms
// Helpers 
// Find the cycle time for a blizzard to come back to its original state 
auto lcm_from_map(size_t n_rows, size_t n_cols) -> size_t {
  
  auto gcd = [](size_t& n_rows, size_t& n_cols) {
    auto max = n_rows > n_cols ? n_rows : n_cols; 
    auto min = n_cols < n_rows ? n_cols : n_rows; 
    
    while (true) {
      auto mod = max % min; 
      if (mod == 0) return min; 
      max = min; min = mod; 
    }
  };
  
  return n_rows * n_cols / gcd(n_rows, n_cols); 
}

// Shortest path algorithm 
// Gives priority to coordinates with shortest time passed and distance from the target coordinate
struct StateComparator {
  bool operator()(const std::pair<Coordinate, int>& s1, const std::pair<Coordinate, int>& s2) const {
    // return s1.second + s1.first.dist_from_target <= s2.second + s2.first.dist_from_target; 
    return s1.second <= s2.second; 
  }
};

// Dijkstra algorithm (uses a std::set as a priority queue)
auto shortest_path(const std::vector<std::string>& map, const Coordinate& start_from, const Coordinate& end_to, int start_time, int time_cycle) -> int {
  
  std::unordered_set<std::string> visited_states; 
  std::set<std::pair<Coordinate, int>, StateComparator> priority_queue; 
  priority_queue.emplace(std::make_pair(start_from, start_time)); 
  
  while (!priority_queue.empty()) {
    // If the next coordinate is the target, return the time spent so far
    auto new_state_it = priority_queue.begin(); 
    
    if (new_state_it->first == end_to) return new_state_it->second; 
    // std::cout << new_state_it->first.to_string() << " -> " << new_state_it->second << std::endl; 
    auto curr_pos = new_state_it->first; 
    auto curr_time = new_state_it->second + 1; 
    priority_queue.erase(new_state_it); // remove the coordinate from the priority queue 
    
    
    
    // Get the current position's neighbors
    auto neighbors = curr_pos.get_neighbors(map.size(), map.at(0).size(), map, curr_time); 
    
    // Add the left coordinates to the priority queue
    for (auto& c: neighbors) {
      if (visited_states.insert(c.to_string() + '-' + std::to_string(curr_time % time_cycle)).second) {
        // c.set_distance(end_to); 
        priority_queue.emplace(std::make_pair(c, curr_time)); 
      }
    }
    
  }
  return std::numeric_limits<int>::max();
}

// Runs the algorithm 
auto find_exit(const std::vector<std::string>& map, bool part_two = false) -> int {
  
  auto time_cycle = lcm_from_map(map.size() - 2, map.at(0).size() - 2); 

  Coordinate end(map.size()-1, map.at(0).size()-2); 
  Coordinate start(0, 1, end);
  end.set_distance(end); 
    
  std::unordered_set<std::string> visited_states; 
  std::set<std::pair<Coordinate, int>, StateComparator> priority_queue; 
  priority_queue.emplace(std::make_pair(start, 0)); 
  
  auto first_round = shortest_path(map, start, end, 0, time_cycle); 
  if (!part_two) return first_round; // for part one we can return the first round 
  // ... but for part second we have to go back to the start and back again! 
  auto second_round = shortest_path(map, end, start, first_round, time_cycle); 
  return shortest_path(map, start, end, second_round, time_cycle); 
}

void solve_part_one(const std::vector<std::string>& map) {
  auto solution = find_exit(map);
  std::cout << "The solution to part one is " << solution << std::endl;
}

void solve_part_two(const std::vector<std::string>& map) {
  auto solution = find_exit(map, true);
  std::cout << "The solution to part two is " << solution << std::endl;
}
