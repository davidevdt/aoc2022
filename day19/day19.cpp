#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <regex> 
#include <unordered_map> 
#include <queue>
#include <cmath> 
#include <algorithm> 

// To solve this day's puzzle, I will use a Depth First Search algorithm, as we want to find the max possible 
// amount of goede among all possible combinations of robots to build and minerals to be picked. In order to 
// obtain a computationally feasible algorithm some optimizations can be applied which take advantage of many 
// robots can (or cannot) be built at each minute. 
// As is, the code currently takes ~17 seconds in Part 2, and so it can be probably further optimized. Test case
// takes much more time in Part 2. 

// This struct will contain all the informaiton we need about the blueprint 
// and the DFS algorithm's current state (number of bots, current amount of minerals picked)
// It also has a max_cost vector which counts what's the maximum amount of minerals to be collected 
struct Blueprint {

    size_t index; 
    std::vector<long int> ore_cost;       // order: ore-clay-obsidian
    std::vector<long int> clay_cost; 
    std::vector<long int> obsidian_cost; 
    std::vector<long int> goede_cost; 
    std::vector<long int> max_cost;  
    std::vector<long int> number_of_bots;       // order: ore bot-clay bot-obsidian bot-goede bot
    std::vector<long int> minerals_amount;      // same order as bots 

    Blueprint (size_t i) noexcept: index(i) {
        ore_cost.assign(3, 0); 
        clay_cost.assign(3, 0); 
        obsidian_cost.assign(3, 0); 
        goede_cost.assign(3, 0); 
        max_cost.assign(3, 0); 
        number_of_bots = {1, 0, 0, 0}; 
        minerals_amount.assign(4, 0); 
    }

    Blueprint(const Blueprint& other) : index(other.index) {
        ore_cost = other.ore_cost; 
        clay_cost = other.clay_cost; 
        obsidian_cost = other.obsidian_cost; 
        goede_cost = other.goede_cost; 
        max_cost = other.max_cost; 
        number_of_bots = other.number_of_bots; 
        minerals_amount = other.minerals_amount; 
    }

    auto print() const {

        std::string prt_str = "Blueprint " + std::to_string(index) + ":\n"; 
        prt_str += "Cost ore = ("; 
        for (const auto& c: ore_cost) prt_str += std::to_string(c) + " "; 
        prt_str += ")\n"; 

        prt_str += "Cost clay = ("; 
        for (const auto& c: clay_cost) prt_str += std::to_string(c) + " "; 
        prt_str += ")\n"; 

        prt_str += "Cost obsidian = ("; 
        for (const auto& c: obsidian_cost) prt_str += std::to_string(c) + " "; 
        prt_str += ")\n"; 

        prt_str += "Cost goede = ("; 
        for (const auto& c: goede_cost) prt_str += std::to_string(c) + " "; 
        prt_str += ")\n"; 

        prt_str += "Max costs = ("; 
        for (const auto& c: max_cost) prt_str += std::to_string(c) + " "; 
        prt_str += ")\n"; 

        std::cout << prt_str << std::endl; 
    }

    // This is useful for creating string keys for hash sets 
    std::string cache_key(long int time) {
        std::string k = std::to_string(time) + "-"; 
        for (const auto& m: minerals_amount) k += std::to_string(m) + "-"; 
        for (const auto& b: number_of_bots) k += std::to_string(b) + "-"; 
        return k;
    }
     

}; 

void get_data(const std::string&, std::vector<Blueprint>&);  
void solve_part_one(const std::vector<Blueprint>&); 
void solve_part_two(const std::vector<Blueprint>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 19 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<Blueprint> blueprints; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, blueprints); 

    if (is_test) {
        for (const auto& b: blueprints) b.print(); 
    }

    solve_part_one(blueprints); 
    solve_part_two(blueprints); 

    return 0; 
}

// Get the input data
void get_data(const std::string& file_name, std::vector<Blueprint>& blueprints) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {

             

            std::vector<std::smatch> matches; 
            // std::regex pattern("\\b(\\d+)\\s*(\\w+)\\b"); 
            std::regex pattern("\\b(\\d+)\\b"); 
            auto begin = std::sregex_iterator(line.begin(), line.end(), pattern); 
            auto end = std::sregex_iterator(); // regex iterator to find all the numbers in the line 

            for (auto it = begin; it != end; ++it) {
                matches.push_back(*it); 
            }
            
            size_t blueprint_number = std::stoul(matches[0][1].str()); 
            Blueprint blueprint(blueprint_number);
            long int ore_cost_ore = std::stol(matches[1][1].str()); 
            long int clay_cost_ore = std::stol(matches[2][1].str()); 
            long int obsidian_cost_ore = std::stol(matches[3][1].str()); 
            long int obsidian_cost_clay = std::stol(matches[4][1].str()); 
            long int goede_cost_ore = std::stol(matches[5][1].str()); 
            long int goede_cost_obsidian = std::stol(matches[6][1].str()); 
            auto max_ore = std::max({ore_cost_ore, clay_cost_ore, obsidian_cost_ore, goede_cost_ore}); 
            auto max_clay = obsidian_cost_clay; 
            auto max_obsidian = goede_cost_obsidian; 

            blueprint.ore_cost.at(0) = ore_cost_ore; 
            blueprint.clay_cost.at(0) = clay_cost_ore; 
            blueprint.obsidian_cost.at(0) = obsidian_cost_ore; 
            blueprint.obsidian_cost.at(1) = obsidian_cost_clay; 
            blueprint.goede_cost.at(0) = goede_cost_ore; 
            blueprint.goede_cost.at(2) = goede_cost_obsidian; 
            blueprint.max_cost.at(0) = max_ore; 
            blueprint.max_cost.at(1) = max_clay; 
            blueprint.max_cost.at(2) = max_obsidian; 

            blueprints.push_back(blueprint); 
            // for (const auto& match: matches) {
                // std::cout << "Number: " << match[1].str() << " Term : " << match[2].str() << std::endl; 
            // }
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// The DFS algorithm. A cache object will keep track of past results 
auto calculate_max_goedes(Blueprint bp, long int time, std::unordered_map<std::string, long int>& cache) {
    
    if (time == 0) {
        return bp.minerals_amount.at(3); 
    }

    auto key = bp.cache_key(time); 
    if (cache.find(key) != cache.end()) return cache[key]; 

    // The current amount of goede is the one already produced plus the one we can produce in the remaining time
    long int max_goede_amount = bp.minerals_amount.at(3) + (bp.number_of_bots.at(3) * time);


    for (size_t mineral_index = 0; mineral_index < 4; ++mineral_index) {
        // The next two lines serve as optimization: for example if the maximum amount of clay we need is 4 units, 
        // and we already have at least 4 clay-collecting robots then building more clay bots is not necessary 
        // Furthermore, if the amount of a mineral we have and can still build is larger than the maximum amount still 
        // required of that mineral, then we can skip the iteration 
        // This is valid only if the mineral we are considering is not the goede mineral 
        if (mineral_index != 3 && bp.number_of_bots.at(mineral_index) >= bp.max_cost.at(mineral_index)) continue; 
        if (mineral_index != 3 && ((bp.number_of_bots.at(mineral_index) * (time)) + bp.minerals_amount.at(mineral_index) >= ((time * bp.max_cost.at(mineral_index))))) continue; 

        long int resource_wait_time = 0; 
        bool robots_are_available = true; 
        std::vector<size_t> minerals_to_collect = {0}; // all minerals require some ore cost 
        std::vector<long int>* curr_mineral_cost; 
        switch (mineral_index) {
            case 0: 
                curr_mineral_cost = &bp.ore_cost; 
                break; 
            case 1: 
                curr_mineral_cost = &bp.clay_cost; 
                break; 
            case 2: 
                curr_mineral_cost = &bp.obsidian_cost; 
                minerals_to_collect.push_back(1); // for obsidian, we also need to consider clay cost 
                break; 
            default: 
                curr_mineral_cost = &bp.goede_cost; 
                minerals_to_collect.push_back(2); // for goede, we also need to consider obsidian cost 
                break; 
        }

        // Check the amount of time needed to build the robot by considering the 
        // max amount of time we need to wait among all resrouces' waiting times that are required to build that bot
        for (const auto& m: minerals_to_collect) {
            if (bp.number_of_bots.at(m) == 0) { // If we don't have robot, we skip
                robots_are_available = false; 
                break; 
            }
            // Amount left to collect per robot 
            resource_wait_time = std::max(resource_wait_time, static_cast<long int>(
                std::ceil(static_cast<double>(curr_mineral_cost->at(m) - bp.minerals_amount.at(m)) / bp.number_of_bots.at(m)) 
            ));
        }

        if (!robots_are_available) continue; 
        auto new_remaining_time = time - resource_wait_time - 1; // -1 to build the robot
        if (new_remaining_time <= 0) continue; // we skip the robot

        // Update the amounts 
        std::vector<long int> updated_amounts; 
        std::transform(bp.minerals_amount.begin(), bp.minerals_amount.end(), bp.number_of_bots.begin(), 
                        std::back_inserter(updated_amounts), [&resource_wait_time](long int curr_amount, long int curr_bots){
            return curr_amount + curr_bots * (resource_wait_time + 1); 
        });

        // Subtract the cost to build the robot
        for (const auto& m: minerals_to_collect) {
            updated_amounts.at(m) -= curr_mineral_cost->at(m); // the cost to build the robot
        }

        // For each updated amount, cap that amount at its maximum cost 
        for (size_t idx = 0; idx < 3; ++idx) {
            updated_amounts.at(idx) = std::min(updated_amounts.at(idx), bp.max_cost.at(idx) * new_remaining_time); 
        }

        // Update the number of bots 
        auto updated_bots = bp.number_of_bots; 
        updated_bots.at(mineral_index) += 1; 

        // New blueprint state for the next round of DFS  
        Blueprint new_state(bp.index); 
        new_state = bp;
        new_state.minerals_amount = updated_amounts; 
        new_state.number_of_bots = updated_bots; 
        max_goede_amount = std::max(max_goede_amount, calculate_max_goedes(new_state, new_remaining_time, cache)); 
    }
    cache[key] = max_goede_amount; 
    return max_goede_amount; 
}

auto calculate_quality_level(const std::vector<Blueprint>& blueprints) {
    long int total_quality_level = 0; 
    for (size_t i = 0; i < blueprints.size(); ++i) {
        std::unordered_map<std::string, long int> cache; 
        total_quality_level += (i+1) * calculate_max_goedes(blueprints.at(i), 24, cache); 
    }
    return total_quality_level; 
}

void solve_part_one(const std::vector<Blueprint>& blueprints) {
    auto solution = calculate_quality_level(blueprints);
    std::cout << "The solution to part one is " << solution << std::endl; 
}

auto calculate_quality_level_part_two(const std::vector<Blueprint>& blueprints) {
    long int total_quality_level = 1; 
    auto max_it = std::min(blueprints.size(), static_cast<size_t>(3)); 
    for (size_t i = 0; i < max_it; ++i) {
        std::unordered_map<std::string, long int> cache; 
        total_quality_level *= calculate_max_goedes(blueprints.at(i), 32, cache); 
    }
    return total_quality_level; 
}

void solve_part_two(const std::vector<Blueprint>& blueprints) {
    auto solution = calculate_quality_level_part_two(blueprints);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


