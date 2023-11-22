#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <utility> 
#include <vector> 
#include <unordered_map> 

using Move = std::pair<char, char>; // a rock-paper-scissor move for each pair of players 

void get_data(const std::string&, std::vector<Move>&); 
auto calculate_scores(const std::vector<Move>&); 
auto convert_move(const std::vector<Move>&); 
void solve_part_one(const std::vector<Move>&); 
void solve_part_two(const std::vector<Move>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 2 <<<" << std::endl; 

    std::vector<Move> strategies; 
    std::string file_name{}; 
    bool is_test{false};
    if (argc > 1 && std::string{argv[1]} == "test") {
        is_test = true; 
    } 

    if (is_test) {
        file_name = "./test.txt";
    } else {
        file_name = "./input.txt";
    }

    get_data(file_name, strategies); 

    if (is_test) {
        for (const auto& s: strategies) {
            std::cout << "Move A: " << s.first << " ";
            std::cout << "Move B: " << s.second << "\n"; 
        }
    }

    solve_part_one(strategies); 
    solve_part_two(strategies); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<Move>& strategies) {

    std::ifstream input_file(file_name); 
    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            char moveA, moveB; 
            std::istringstream line_stream(line);
            line_stream >> moveA >> moveB; 
            // std::cout << "Move A: " << moveA << ", Move B: " << moveB << std::endl; 
            Move newMove(moveA, moveB); 
            strategies.push_back(newMove); 
        }
    } else {
        std::cerr << "File not found." << std::endl; 
        return; 
    }
    input_file.close(); 
}

auto calculate_scores(const std::vector<Move>& strategies) {

    // First player: A = rock; B = paper; C = scissors; 
    // Second player: X = rock (1); Y = paper (2); Z = scissors (3); 
    // Loss = 0; Draw = 3; Win = 6; 

    int total_score{0}; 

    auto calculate_match_result = [](char moveA, char moveB) -> char {
        if (moveA == 'A') {
            if (moveB == 'X') return 'D'; 
            if (moveB == 'Y') return 'W'; 
            if (moveB == 'Z') return 'L'; 
        } 
        
        if (moveA == 'B') {
            if (moveB == 'X') return 'L'; 
            if (moveB == 'Y') return 'D'; 
            if (moveB == 'Z') return 'W'; 
        } 

        // moveA == 'C'
        if (moveB == 'X') return 'W'; 
        if (moveB == 'Y') return 'L'; 
        return 'D'; // moveB == 'Z'
    }; 

    std::unordered_map <char, int> move_points; 
    std::unordered_map <char, int> match_points; 

    move_points['X'] = 1; 
    move_points['Y'] = 2; 
    move_points['Z'] = 3; 
    match_points['L'] = 0; 
    match_points['D'] = 3; 
    match_points['W'] = 6; 
    
    for (const auto& move: strategies) {
        total_score += move_points[move.second]; 
        auto match_result = calculate_match_result(move.first, move.second); 
        total_score += match_points[match_result]; 
    }

    return total_score; 
} 

void solve_part_one(const std::vector<Move>& strategies) {
    auto strategy_score = calculate_scores(strategies); 
    std::cout << "The score for part one is " << strategy_score << std::endl; 
}

// For part two 
// Convert the moves according to W-D-L to make it conformant 
// to the algorithm of part one 
auto convert_move(const std::vector<Move>& strategies) {
    auto new_strategies = strategies;
    
    for (auto& s: new_strategies) {

        if (s.first == 'A') {
            if (s.second == 'X') s.second = 'Z';
            else if (s.second == 'Y') s.second = 'X'; 
            else if (s.second == 'Z') s.second = 'Y';  
        } else if (s.first == 'B') {
            if (s.second == 'X') s.second = 'X'; 
            else if (s.second == 'Y') s.second = 'Y'; 
            else if (s.second == 'Z') s.second = 'Z';             
        } else if (s.first == 'C') {
            if (s.second == 'X') s.second = 'Y'; 
            else if (s.second == 'Y') s.second = 'Z'; 
            else if (s.second == 'Z') s.second = 'X';  
        }
    }

    return new_strategies; 
}

void solve_part_two(const std::vector<Move>& strategies) {
    auto strategy_score = calculate_scores(convert_move(strategies)); 
    std::cout << "The score for part two is " << strategy_score << std::endl; 
}