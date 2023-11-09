#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <unordered_set> 
#include <queue>

// For Day 19, I create a Cube struct which contains the coordinates x, y, z. To solve part I's puzzle, I put all the 
// input cubes inside a hash set, and check for each cube's face whether there are cubes adjacent to it. If so, then the 
// surface is excluded from the grand total. For Part II, I calculate a sort of "min" and "max" cubes, which contains the 
// min and max extremes of the all the cubes in my input set. Then it uses a sort of "breadth-first-search" algorithm to 
// check all the possible cubes that can connect this min and max cube. If, during the search, one of the cubes in the input
// set appears, then the count is increased by one. The search ends when all the possible combinations of adjacent cubes have 
// been explored. 

struct Cube {
    int x, y, z; 

    explicit Cube(int a, int b, int c) noexcept : x(a), y(b), z(c) {}

    // Useful to create a hash set of cubes
    struct CubeHash {
        size_t operator()(const Cube& c) const {
            auto h1 = std::hash<int>{}(c.x);
            auto h2 = std::hash<int>{}(c.y); 
            auto h3 = std::hash<int>{}(c.z); 
            return h1 ^ h2 ^ h3;  
        }
    };

    // Also needed for the hash set 
    bool operator==(const Cube& other) const {
        return x == other.x && y == other.y && z == other.z; 
    }

    void print() const {
        std::cout << "(" << x << "," << y << "," << z << ")" << std::endl; 
    }
};


void get_data(const std::string&, std::unordered_set<Cube, Cube::CubeHash>&);  
void solve_part_one(const std::unordered_set<Cube, Cube::CubeHash>&); 
void solve_part_two(const std::unordered_set<Cube, Cube::CubeHash>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 18 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::unordered_set<Cube, Cube::CubeHash> cubes; 


    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, cubes); 

    if (is_test) {
        for (const auto& c: cubes) c.print(); 
    }

    solve_part_one(cubes); 
    solve_part_two(cubes); 

    return 0; 
}

// Get the input data
void get_data(const std::string& file_name,std::unordered_set<Cube, Cube::CubeHash>& cubes) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            std::istringstream line_stream(line); 
            int x, y, z; 
            line_stream >> x; 
            line_stream.ignore(); 
            line_stream >> y; 
            line_stream.ignore(); 
            line_stream >> z; 
            Cube cube(x, y, z); 
            cubes.insert(cube); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
auto count_disconnected_surfaces(const std::unordered_set<Cube, Cube::CubeHash>& cubes) {

    long int n_disconnected_surfaces = 0; 

    for (auto& c: cubes) {
        // Counts how many of the adjacent cubes are NOT in the cubes set
        if (cubes.find(Cube(c.x+1, c.y, c.z)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

        if (cubes.find(Cube(c.x-1, c.y, c.z)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

        if (cubes.find(Cube(c.x, c.y+1, c.z)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

        if (cubes.find(Cube(c.x, c.y-1, c.z)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

        if (cubes.find(Cube(c.x, c.y, c.z+1)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

        if (cubes.find(Cube(c.x, c.y, c.z-1)) == cubes.end()) {
            n_disconnected_surfaces += 1; 
        }

    }

    return n_disconnected_surfaces; 

}

void solve_part_one(const std::unordered_set<Cube, Cube::CubeHash>& cubes) {
    auto solution = count_disconnected_surfaces(cubes);
    std::cout << "The solution to part one is " << solution << std::endl; 
}

auto count_surfaces_exposed_cubes(const std::unordered_set<Cube, Cube::CubeHash>& cubes) {

    // Find max and min x, y, z and create a min and max cubes
    int min_x, max_x, min_y, max_y, min_z, max_z;
    bool is_start = true; 
    long int exposed_outside = 0; 

    for (const auto& c: cubes) {
        if (is_start) {
            min_x = c.x; max_x = c.x; min_y = c.y; max_y = c.y; min_z = c.z; max_z = c.z; 
            is_start = false; 
        }

        if (c.x < min_x) min_x = c.x; 
        else if (c.x > max_x) max_x = c.x; 
        if (c.y < min_y) min_y = c.y; 
        else if (c.y > max_y) max_y = c.y; 
        if (c.z < min_z) min_z = c.z; 
        else if (c.z > max_z) max_z = c.z;  
    } 

    Cube min_cube = Cube(min_x - 1, min_y -1, min_z -1); 
    Cube max_cube = Cube(max_x + 1, max_y + 1, max_z + 1);

    // Check through a queue all possible combinations of adjacent cubes in the exposed part of the surface
    // and count how many of the input cubes appear from the considered surfaces 
    std::unordered_set<Cube, Cube::CubeHash> visited; 
    std::queue<Cube> cube_queue; 
    cube_queue.push(max_cube); 

    while(!cube_queue.empty()) {

        auto new_cube = cube_queue.front(); 
        cube_queue.pop(); 

        // Note that this increases only when a cube appears in one of the six exterior surface cubes
        if (cubes.find(new_cube) != cubes.end()) {
            ++exposed_outside; 
            continue; 
        }

        if (visited.find(new_cube) == visited.end()) {
            visited.insert(new_cube); 

            auto tmp_cube = Cube(new_cube.x + 1, new_cube.y, new_cube.z); 
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
            tmp_cube.x = new_cube.x - 1; 
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
            tmp_cube.x = new_cube.x; tmp_cube.y = new_cube.y + 1; 
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
            tmp_cube.y = new_cube.y - 1; 
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
            tmp_cube.y = new_cube.y; tmp_cube.z = new_cube.z + 1; 
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
            tmp_cube.z = new_cube.z - 1;  
            if (tmp_cube.x >= min_cube.x && tmp_cube.x <= max_cube.x && \
                tmp_cube.y >= min_cube.y && tmp_cube.y <= max_cube.y && \
                tmp_cube.z >= min_cube.z && tmp_cube.z <= max_cube.z) cube_queue.push(tmp_cube); 
        }
    }
    return exposed_outside; 
}

void solve_part_two(const std::unordered_set<Cube, Cube::CubeHash>& cubes) {
    auto solution = count_surfaces_exposed_cubes(cubes);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


