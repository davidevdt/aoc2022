#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <algorithm>
#include <regex>  
#include <unordered_map> 
#include <unordered_set> 
#include <limits> 
#include <utility> 
#include <set> 

// For Day 16, I will create a Valve struct that will store all the relevant information about the input data. [Note: as the valves 
// are stored in a std::unordered_map in which the keys are their names, the name field is not really relevant here. This must be changed then]
// First, I will get the input data. Then I will calculate the distances between the valves using a shortest path algorithm, 
// in this case Dijkstra (I will use a std::set as a priority queue).  
// Then, for part one I will find the maximum relieved pressur with the use of recursion and caching (making it as a matter of fact
// a dynamic programming solution). For part two, I will use the same simulation funciton, however I will introduce a ele_cache
// object which keeps track of how much max relieved pressure each combination of opened valves leads to. This will be useful 
// to find the disjoint sets of valves which maximize the relieved pressure, and allow to find the best combination of pressure
// of the human + elephant paths within the 26 minutes. The valves with 0 flows will be skipped to speed up computations.


struct Valve {

    std::string name; 
    unsigned int flow_rate; 
    std::unordered_set<std::string> neighbors; 

    explicit Valve(std::string n, unsigned int f, std::unordered_set<std::string> nb) noexcept: 
        name(n), flow_rate(f), neighbors(nb) {}

    explicit Valve(std::string n) noexcept: name(n), flow_rate(0) {
        neighbors = std::unordered_set<std::string>(); 
    } 

    auto print() const {
        std::cout << "Valve " << name << " : flow rate = " << std::to_string(flow_rate) << " -> "; 
        for (const auto& v: neighbors) {
            std::cout << "(" << v << ") "; 
        }
        std::cout << std::endl; 
    }

    // // The following operations are useful if we want to use the valves as keys for hash sets or maps 
    // bool operator==(const Valve& other) const {
    //     return this->name == other.name; 
    // }

    // struct HashFunction {
    //     size_t operator()(const Valve& v) const {
    //         return std::hash<std::string>()(v.name); 
    //     }
    // }; 

};

using ValvesMap = std::unordered_map<std::string, Valve>; 

// The following two struct's will be used as hashing and equality functions 
// when we will use std::unordered_set's as keys for the std::unordered_maps in Part 2 (the ele_cache). 
struct HashSetHash {
    size_t operator()(const std::unordered_set<std::string>& set) const {
        std::size_t hash = 0;
        for (const auto& element : set) {
            hash ^= std::hash<std::string>()(element);
        }
        return hash;
    }
};

struct HashSetEqual {
    bool operator()(const std::unordered_set<std::string>& set1, const std::unordered_set<std::string>& set2) const {
        return set1 == set2;
    }
};

void get_data(const std::string&, ValvesMap&);  
void solve_part_one(ValvesMap&); 
void solve_part_two(ValvesMap&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 16 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    ValvesMap valves; 


    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, valves); 

    if (is_test) {
        for (const auto& v: valves) v.second.print(); 
    }

    solve_part_one(valves); 
    solve_part_two(valves); 

    return 0; 
}

// Get the data, using regex 
void get_data(const std::string& file_name, ValvesMap& valves) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        std::regex pattern(R"(Valve ([A-Z]{2}) has flow rate=(\d+); \btunnels?\b \bleads?\b to \bvalves? (([A-Z]+, ?)*[A-Z]+))"); 
        std::smatch match; 
        while (std::getline(input_file, line)) {
            std::unordered_set<std::string> neighboring_valves; 
            std::string::const_iterator search_it(line.cbegin());
            if (std::regex_search(search_it, line.cend(), match, pattern) && match.size() >= 4) { 
                std::string valve_name = match[1].str(); 
                unsigned int flow_rate = std::stoi(match[2].str()); 
                
                // Check if there are one or more valves the current valve is leading to 
                size_t start=0; 
                while (true) {
                    auto found_comma = match[3].str().find(',', start); 
                    if (found_comma != std::string::npos) {
                        neighboring_valves.insert(match[3].str().substr(start, found_comma-start)); 
                        start = found_comma + 2; // skips also the space 
                    } else {
                        neighboring_valves.insert(match[3].str().substr(start)); 
                        break; 
                    }
                }
                Valve valve(valve_name, flow_rate, neighboring_valves); 
                valves.insert(std::make_pair(valve_name, valve)); 
            }
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// This function will be used as a comparison function for the priority queue of the shortest path algorithm
struct ValvesDistanceComparator {
    bool operator()(const std::pair<std::string, unsigned int>& v1, const std::pair<std::string, unsigned int>& v2) const {
        return v1.second <= v2.second; 
    }
};

// Dijkstra algorithm: using a priority queue, it finds the shortest distance in the graph 
// between the source and the destination valve. This is useful because it will allow us to 
// maximize the amount of valves we can visit within the 30 minutes 
auto shortest_path(const Valve& source, const Valve&  destination, ValvesMap& valves) {

    std::set<std::pair<std::string, unsigned int>, ValvesDistanceComparator> priority_queue; 
    // std::string prev{""}; 

    std::unordered_set<std::string> visited_valves;
    visited_valves.insert(source.name);  
    priority_queue.insert(std::make_pair(source.name, 0)); 

    while (!priority_queue.empty()) {
        auto current_node_it = priority_queue.begin(); 
        if (current_node_it->first == destination.name) return current_node_it->second; 

        std::pair<std::string,int> current_node = {current_node_it->first, current_node_it->second};
        priority_queue.erase(current_node_it); 
        auto valve_it = valves.find(current_node.first); 
        auto neighbors = valve_it->second.neighbors; 
        
        for (const auto& neighbor_node: neighbors) {
            if (visited_valves.insert(neighbor_node).second) {
                auto it = std::find_if(priority_queue.begin(), priority_queue.end(), [&neighbor_node](auto& v1){
                    return v1.first == neighbor_node;
                });
                unsigned int dist = current_node.second + 1;
                if (it != priority_queue.end()) {
                    priority_queue.erase(it); 
                }
                auto is_inserted = priority_queue.insert(std::make_pair(neighbor_node, dist)); 
            }
        }
    }
    return std::numeric_limits<unsigned int>::max(); 
}

// Calculates the shortest distance between all valves in the volcano. It returns such distances in terms of 
// a std::unordered_map
auto calculate_valve_distances(ValvesMap& valves) {

    std::unordered_map<std::string, unsigned int> valves_distances;
    auto valves_it = valves.cbegin(); 

    while (valves_it != valves.cend()) {
        auto valves_it_second = valves.cbegin(); 
        if (valves_it->second.flow_rate == 0 && valves_it->first != "AA") {++valves_it; continue;} 
        while (valves_it_second != valves.cend()) {
            if (valves_it_second->second.flow_rate == 0) {++valves_it_second; continue;} 
            std::string from_to = valves_it->first + '-' + valves_it_second->first;
            std::string to_from = valves_it_second->first + '-' + valves_it->first; 
            if (valves_distances.find(from_to) == valves_distances.end()) {
                if (valves_it->first != valves_it_second->first) {
                    auto dist = shortest_path(valves_it->second, valves_it_second->second, valves);
                    valves_distances[from_to] = dist; 
                    dist = shortest_path(valves_it_second->second, valves_it->second, valves);
                    valves_distances[to_from] = dist; 
                } else {
                    valves_distances[from_to] = 0; 
                    valves_distances[to_from] = 0; 
                }
            }
            ++valves_it_second; 
        }
        ++valves_it; 
    }

    // for (const auto it: valves_distances) {
    //     std::cout << "distance " << it.first << " is " << it.second << std::endl; 
    // }

    return valves_distances; 
}

// Input of the simulation function. This is used here because it allows me to write the input in a compact way, 
// besides allowing for easy caching of the intermediate results to speed up the algorithm.. 
struct CurrentState {
    std::string valve_from; 
    std::unordered_set<std::string> opened_valves; 
    unsigned int elapsed_time; 
    unsigned int pressure_relieved; 

    CurrentState(std::string vn, unsigned int et, unsigned int tpr): valve_from(vn), elapsed_time(et), pressure_relieved(tpr) {} 
    CurrentState(): valve_from("AA"), elapsed_time(0), pressure_relieved(0) {} // {path.push_back("AA");}

    auto opened_to_string() const {
        std::string opened_valves_string; 
        auto it = opened_valves.begin();  
        while (it!= opened_valves.end()) {
            opened_valves_string += *it;
            opened_valves_string += "-"; 
            ++it; 
        }
        return opened_valves_string; 
    }

    auto to_string() const {
        std::string state_string = valve_from + '-' + std::to_string(elapsed_time) + '-' + std::to_string(pressure_relieved) + '-'; 
        state_string += this->opened_to_string(); 
        return state_string; 
    }

}; 

// The simulation function. It recursively calls itself as follows: 
// first, it checks if given the curent state, the 30 (or 26) minutes have passed, or if all the valves having flow != 0 are opened
// If that's the case, it calcualtes how many minutes we still have left, and calculates the pressure to relieve until we reach max_time
// Second, it loops through the unopened valves, updates the current total pressure, how many new minutes have elapsed (given by 
// the distance from the source to the destination valve), and updates the new set of open valves 
// Last, it recursively calls itself to calculate the new updated pressure. It returns the maximizing pressure 
// For Part 2, the ele_cache (pointer) map keeps track of the sets of open valves and the corresponding maximized pressure. 
// For Part 1, this element defaults to a null pointer. 
auto find_max_relief(CurrentState current_state, const ValvesMap& valves, 
                     std::unordered_set<std::string>& flowing_valves,
                     std::unordered_map<std::string, unsigned int>& valves_distances, 
                     std::unordered_map<std::string, unsigned int>& cache, unsigned int max_time = 30, 
                     std::unordered_map<std::unordered_set<std::string>, unsigned int, HashSetHash, HashSetEqual>* ele_cache=nullptr) -> unsigned int {

    auto css = current_state.to_string(); 
    if (cache.find(css) != cache.end()) return cache.find(css)->second;  

    unsigned int current_total_pressure_relieved = 0; 
    for (const auto& v: current_state.opened_valves) current_total_pressure_relieved += valves.find(v)->second.flow_rate; 

    // Check if the elapsed time is over, the number of opened valves is to the max, or if we need to cache the results for part 2
    if ((current_state.elapsed_time >= max_time || current_state.opened_valves.size() == flowing_valves.size()) || (ele_cache)) {
        auto pressure_per_minute = current_total_pressure_relieved; 
        auto total_pressure = current_state.pressure_relieved + (pressure_per_minute * (max_time - current_state.elapsed_time));
        cache[current_state.to_string()] = total_pressure;  

        if (ele_cache) {
            if (!ele_cache->insert(std::make_pair(current_state.opened_valves, total_pressure)).second) {
                if (total_pressure > (*ele_cache)[current_state.opened_valves]) (*ele_cache)[current_state.opened_valves] = total_pressure; 
            }  
        }
        else return total_pressure; // In part two, returning here would mean to stop the algorithm at the first iteration, we just let it go 
    }
    
    // This is another potential solution but I need to work on that as it leads to the wrong result as is 
    // auto max_pressure = !ele_cache ? 0 : find_max_relief(CurrentState(), valves, flowing_valves, valves_distances, cache, max_time);    
    auto max_pressure = 0; 

    // Find the unopened valves and loop through them 
    std::unordered_set<std::string> unopened_valves; 
    for (auto& fw: flowing_valves) if (current_state.opened_valves.find(fw) == current_state.opened_valves.end()) unopened_valves.insert(fw); 

    for (const auto& next_valve: unopened_valves) {
        // Update the distance, elapsed time, and pressure relieved for each valve we need to visit
        auto dist = valves_distances[current_state.valve_from + '-' + next_valve] + 1; // +1 because we're opening the valve
        auto new_elapsed_time = current_state.elapsed_time + dist;   
        auto new_pressure_relieved = current_total_pressure_relieved *  dist;  
        new_pressure_relieved += current_state.pressure_relieved; 
        
        // Check if the elaped time is over: if so simply check if the max pressure needs to be updated 
        if (new_elapsed_time >= max_time) {
            auto pressure_per_minute = current_total_pressure_relieved; 
            auto total_pressure = current_state.pressure_relieved + (pressure_per_minute * (max_time - current_state.elapsed_time));
            if (total_pressure > max_pressure) max_pressure = total_pressure; 
            continue;   
        }

        // Recursively call the function with the new updated state 
        CurrentState new_state{next_valve, new_elapsed_time, new_pressure_relieved}; 
        new_state.opened_valves = current_state.opened_valves;
        new_state.opened_valves.insert(next_valve);  // updates the opened valves set 
        auto new_pressure = find_max_relief(new_state, valves, flowing_valves, valves_distances, cache, max_time, ele_cache); 
        // Check if the pressure has a new maximum and cache the result for fast retrieval  
        if (new_pressure > max_pressure) max_pressure = new_pressure; 
        cache[new_state.to_string()] = max_pressure;
    }

    return max_pressure; 
}


// This solution (for Part 1) is equivalent; in my opinion, a bit more intuitive but also a bit slower. 
// At each recursive call, it keeps track of how much new steam has been released by the new valve, 
// rather than keeping track of the total amount of steam released at each step by all valves. 
// auto find_max_relief(CurrentState current_state, const ValvesMap& valves, 
//                         std::unordered_set<std::string>& flowing_valves,
//                         std::unordered_map<std::string, unsigned int>& valves_distances, 
//                         std::unordered_map<std::string, unsigned int>& cache, unsigned int max_time = 30) -> unsigned int {

//     auto css = current_state.to_string(); 
//     if (cache.find(css) != cache.end()) return cache.find(css)->second;  

//     if (current_state.elapsed_time > max_time || current_state.opened_valves.size() == flowing_valves.size()) {
//         auto css = current_state.to_string(); 
//         cache[css] = 0;
//         return 0;  
//     }

//     unsigned int max_pressure = 0; 
//     unsigned int new_pressure = 0; 
//     auto new_opened = current_state.opened_valves; 
//     auto new_elapsed_time = current_state.elapsed_time + 1; 

//     std::unordered_set<std::string> unopened_valves; 
//     for (auto& fw: flowing_valves) if (current_state.opened_valves.find(fw) == current_state.opened_valves.end()) unopened_valves.insert(fw); 
    
//     if (unopened_valves.find(current_state.valve_from) != unopened_valves.end()) {
//         new_pressure = valves.find(current_state.valve_from)->second.flow_rate * (max_time - new_elapsed_time); 
//         new_opened.insert(current_state.valve_from); 
//     }

//     for (const auto& next_valve: flowing_valves) {
//         if (next_valve == current_state.valve_from) continue; 
//         auto dist = valves_distances[current_state.valve_from + '-' + next_valve]; 

//         CurrentState new_state{next_valve, new_elapsed_time + dist - (new_pressure == 0), new_pressure}; 
//         new_state.opened_valves = new_opened; 

//         auto path_pressure = find_max_relief(new_state, valves, flowing_valves, valves_distances, cache);  
//         cache[new_state.to_string()] = path_pressure;  
//         if ((new_pressure + path_pressure) > max_pressure) max_pressure = (new_pressure + path_pressure); 
//     }

//     return max_pressure; 
// }

// Part one: creates the inputs and run the simulation; returns the result
auto run_part_one(ValvesMap& valves) {
    auto valves_distances = calculate_valve_distances(valves); 
    auto number_of_zero_flow_valves = 0; 
    std::unordered_map<std::string, unsigned int> cache; 
    CurrentState initial_state; 
    std::unordered_set<std::string> flowing_valves;
    for (auto& fw: valves) if (fw.second.flow_rate != 0) flowing_valves.insert(fw.first); 
    auto max_relief = find_max_relief(initial_state, valves, flowing_valves, valves_distances, cache);
    return max_relief;    
}


void solve_part_one(ValvesMap& valves) {
    // auto solution = find_max_relief(valves); 
    auto solution = run_part_one(valves); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// Part two: creates the inputs and run the simulation; we use the ele_cache object to find the solution
// In particular we need to find the two disjoint sets of opened valves that maximize the relieved pressure 
// This is because the elefan and the human cannot open the same valve twice 
auto run_part_two(ValvesMap& valves) {
    auto valves_distances = calculate_valve_distances(valves); 
    auto number_of_zero_flow_valves = 0; 
    std::unordered_map<std::string, unsigned int> cache; 
    CurrentState initial_state; 
    std::unordered_set<std::string> flowing_valves;
    for (auto& fw: valves) if (fw.second.flow_rate != 0) flowing_valves.insert(fw.first); 
    std::unordered_map<std::unordered_set<std::string>, unsigned int, HashSetHash, HashSetEqual> ele_cache; 
    find_max_relief(initial_state, valves, flowing_valves, valves_distances, cache, 26, &ele_cache); 

    auto outer_it = ele_cache.begin(); 
    auto max_pressure = 0; 
    while(outer_it != ele_cache.end()) {
        if (outer_it == ele_cache.end()) break; 
        auto inner_it = std::next(outer_it); 
        while (inner_it != ele_cache.end()) {
            if (outer_it->first == inner_it->first) {++inner_it; continue;}

            // Check if the two sets are disjoint (they shouldn't have elements in common as the human
            // and the elephant should maximize two different paths)
            bool disjoint = true; 
            for (const auto& outer_elements: outer_it->first) {
                if (inner_it->first.find(outer_elements) != inner_it->first.end()) {
                    disjoint = false; break; 
                }
            }
            if (!disjoint) {++inner_it; continue;}
            auto tmp_pressure = outer_it->second + inner_it->second; 
            if (tmp_pressure > max_pressure) max_pressure = tmp_pressure; 
            ++inner_it; 
        }
        ++outer_it; 
    }
    return max_pressure; 
}

void solve_part_two(ValvesMap& valves) {
    auto solution = run_part_two(valves);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


