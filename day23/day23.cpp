#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <unordered_map>
#include <limits>

/* For today's task, I will create and Elf struct (which really is a coordinate container) 
   that will implement the following methods: 
   - one that generate all neighboring Elves in the 8 surrounding directions
   - one that generates a neighbor in a specific direction
   - one that accepts/rejects a move based on the surrounding neighbors 
   - an equality operator (useful for STL unordered containers)
   - a struct with the hashing operation (also needed for sets and maps keys)
 
 The elves in the map will be stored into a std::unordered_set. To help, I will use an enum class containing 
 the 8 directions that we need to check.
  For Part 1 and 2, the algorithm will simply perform the following steps: 
  (a) For each elf, it checks if all surrounding positions are free; if not, 
  (b) it generates all the elf's neighbors, and checks into which direction he can move based on those 
  (c) if there is a suitable proposed direction, it adds it to a dedicated hashmap 
  (d) once all the elves are done, it checks how many elves decide to move to a specific direction: 
      if such count is only one, then we perform the move for that elf 
  (e) steps a-d are iterated for ten times for part one, at  the end of which the are of the rectangle 
      in which the elves are included is calculated; for part two, it keeps iterating until there's no more moves performed
*/


enum class Dirs {
  N, NE, E, SE, S, SW, W, NW
};

constexpr const auto MAX_LONG = std::numeric_limits<long int>::max();  
constexpr const auto MIN_LONG = std::numeric_limits<long int>::min();  

struct Elf {
  
  // Hashing struct 
  struct ElfHash {
    long int operator()(const Elf& e) const {
      auto first_hash = std::hash<long int>()(e.row); 
      auto second_hash = std::hash<long int>()(e.col); 
      return first_hash ^ second_hash; 
    }
  }; 
  
  long int row, col; 
  
  Elf(): row(0), col(0) {}
  explicit Elf(long int r, long int c): row(r), col(c) {}
  
  auto print() const -> void {
    std::cout << "(" << row << "," << col << ")" << std::endl; 
  }
  
  // Gets all the Elf's neighbors 
  auto get_neighbors() const -> std::vector<Elf> {
    std::vector<Elf> neighbors(8); 
    neighbors.at(0) = get_neighbor(Dirs::N);
    neighbors.at(1) = get_neighbor(Dirs::NE);
    neighbors.at(2) = get_neighbor(Dirs::E);
    neighbors.at(3) = get_neighbor(Dirs::SE);
    neighbors.at(4) = get_neighbor(Dirs::S);
    neighbors.at(5) = get_neighbor(Dirs::SW);
    neighbors.at(6) = get_neighbor(Dirs::W);
    neighbors.at(7) = get_neighbor(Dirs::NW);
    return neighbors; 
  }
  
  // Gets a neighbor in a specific direction 
  auto get_neighbor(Dirs d) const -> Elf {
    
    switch(d) {
    case Dirs::N:
      return Elf(this->row-1, this->col);
    case Dirs::NE: 
      return Elf(this->row-1, this->col+1); 
    case Dirs::E: 
      return Elf(this->row, this->col+1); 
    case Dirs::SE: 
      return Elf(this->row+1, this->col+1); 
    case Dirs::S: 
      return Elf(this->row+1, this->col); 
    case Dirs::SW: 
      return Elf(this->row+1, this->col-1); 
    case Dirs::W: 
      return Elf(this->row, this->col-1); 
    case Dirs::NW: 
      return Elf(this->row-1, this->col-1); 
    default: // Dirs::NONE
      return *this; 
    }
  }
  
  // Given a vector of boolean's (which flags which neighboring positions are occupied)
  // and a direction into which we desire to move, it returns true if we can move into that
  // direction, and false otherwise 
  auto can_move_to(Dirs d, std::vector<bool> neighbors_are_occupied) const -> bool {
    
    // Note: the order of the directions is assumed to be N-NE-E-SE-S-SW-W-NW
    switch(d) {
    case Dirs::N: 
      return !neighbors_are_occupied.at(0) && !neighbors_are_occupied.at(1) && !neighbors_are_occupied.at(7); 
    case Dirs::E: 
      return !neighbors_are_occupied.at(1) && !neighbors_are_occupied.at(2) && !neighbors_are_occupied.at(3);
    case Dirs::S: 
      return !neighbors_are_occupied.at(3) && !neighbors_are_occupied.at(4) && !neighbors_are_occupied.at(5);
    case Dirs::W: 
      return !neighbors_are_occupied.at(5) && !neighbors_are_occupied.at(6) && !neighbors_are_occupied.at(7);
    default: // Dirs::W
      return false;
    }
  }
  
  // Equality operator 
  bool operator==(const Elf& other) const {
    return this->row == other.row && this->col == other.col; 
  }
  
};

void get_data(const std::string&, std::unordered_set<Elf, Elf::ElfHash>&);  
void solve_part_one(std::unordered_set<Elf, Elf::ElfHash>); 
void solve_part_two(std::unordered_set<Elf, Elf::ElfHash>); 

int main(int argc, char* argv[]) {
  
  std::cout << ">>> Advent Of Code 2022 - Day 23 <<<" << std::endl; 
  
  std::string file_name; 
  bool is_test = false; 
  std::unordered_set<Elf, Elf::ElfHash> elves;
  
  if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
  if (is_test) file_name = "./test.txt"; 
  else file_name = "./input.txt"; 
  
  get_data(file_name, elves); 
  
  if (is_test) {
    std::cout << "Elves: " << std::endl; 
    for (const auto& e: elves) e.print(); 
  }
  
  solve_part_one(elves); 
  solve_part_two(elves); 
      
  return 0; 
}


// Get the input data
void get_data(const std::string& file_name, std::unordered_set<Elf, Elf::ElfHash>& elves) {
  
  std::ifstream input_file{file_name}; 
  
  if (input_file.is_open()) {
    std::string line; 
    long int row_counter = 0; 
    while (std::getline(input_file, line)) {
      for (long int col_counter = 0; col_counter < line.size(); ++col_counter) {
        if (line.at(col_counter) == '#') {
          Elf elf(row_counter, col_counter);
          elves.insert(elf); 
        }
      }
      ++row_counter; 
    }
  } else {
    std::cerr << "Could not open the file." << std::endl; 
    exit(0); 
  }
  input_file.close(); 
} 

// Algorithms
// Moves the elves around the map 
auto move_elves(std::unordered_set<Elf, Elf::ElfHash>& elves, bool part_two = false) -> long int {
  
  std::vector<Dirs> directions_to_check = {Dirs::N, Dirs::S, Dirs::W, Dirs::E};
  size_t starting_checking_index = 0; // this will be updated at each loop iteration to rotate the starting direction proposal value
  unsigned int it_counter = 0; 
  
  while (true) {
    
    bool elves_are_moving = false;
    std::unordered_map<Elf, std::vector<Elf>, Elf::ElfHash> move_proposals; // here I will store the potential moves (keys) and the proposers (vector of elves, the value)
    
    // Loop through the elves
    for (auto& e: elves) {
      auto neighbors = e.get_neighbors(); // the elf's neighbors
      
      // If there are no neighbors in the map, skip the elf 
      if (std::all_of(neighbors.cbegin(), neighbors.cend(), [&elves](const Elf& n) {
        return elves.find(n) == elves.end(); 
      })) {
        continue; 
      }
      
      // Check which elves in the map are in the neighborhood
      std::vector<bool> neighbors_are_occupied; 
      for (const auto& n: neighbors) {
        neighbors_are_occupied.push_back(elves.find(n) != elves.end()); 
      }
      
      // Checks if the elf can move to a specific direction; the circularity 
      // is ensured by placing an offset and then modulo the index 
      for (size_t offset = 0; offset < 4; ++offset) {
        auto direction_index = (starting_checking_index + offset) % 4;
        auto d = directions_to_check.at(direction_index); 
        if (e.can_move_to(d, neighbors_are_occupied)) {
          auto new_elf_move = e.get_neighbor(d); 
          move_proposals[new_elf_move].push_back(e);
          break; 
        }
      }
    }

   // Move the elves who alone proposed a specific new move
   for (const auto& m: move_proposals) {
     if (m.second.size() == 1) {
       elves_are_moving = true; // for part 2: if even just one elf is moving, flag it 
       elves.erase(elves.find(m.second.at(0))); 
       elves.insert(m.first); 
     }
   }
   
  // Solution part 1: calculate the empty spaces in the elves' rectangle 
  if (!part_two && it_counter == 9) { // stop at 9 as we are starting the counter at 0
    auto max_row = MIN_LONG; 
    auto max_col = MIN_LONG; 
    auto min_row = MAX_LONG; 
    auto min_col = MAX_LONG; 
    
    for (const auto& e: elves) {
      if (e.row < min_row) min_row = e.row; 
      if (e.row > max_row) max_row = e.row; 
      if (e.col < min_col) min_col = e.col; 
      if (e.col > max_col) max_col = e.col; 
    }
    return ((max_row - min_row + 1) * (max_col - min_col + 1) - elves.size()); 
  }
  
  // For part two: returns the iteration number when no elf has moved 
  if (part_two && !elves_are_moving) return it_counter + 1; // +1 because we start counting from 0
  starting_checking_index = (starting_checking_index + 1) % 4;
  ++it_counter; 
  }
  
  return MAX_LONG;
}
  
void solve_part_one(std::unordered_set<Elf, Elf::ElfHash> elves) {
  auto solution = move_elves(elves);
  std::cout << "The solution to part one is " << solution << std::endl; 
}

void solve_part_two(std::unordered_set<Elf, Elf::ElfHash> elves) {
  auto solution = move_elves(elves, true);
  std::cout << "The solution to part two is " << solution << std::endl; 
}
