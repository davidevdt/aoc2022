#include <string>
#include <sstream>
#include <fstream> 
#include <iostream>
#include <vector> 
#include <utility> 

void get_data(const std::string&, std::vector<std::string>&); 
void solve_part_one(const std::vector<std::string>&); 
void solve_part_two(const std::vector<std::string>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 10 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<std::string> instructions; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, instructions); 

    if (is_test) {
        for (const auto& i: instructions) {
            std::cout << i << std::endl; 
        }
    }

    solve_part_one(instructions); 
    solve_part_two(instructions); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<std::string>& instructions) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            instructions.push_back(line); 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

} 

// Approach: loop through each instructio, increase the cycle number according to 
// the instruction type. After each increase, check if we are at one of the cycles
// we need to record. If that's the case, get the update register value and muliply it 
// with the corresponding cycle number.
auto calculate_signal_strength(const std::vector<std::string>& instructions) {

    size_t cycle = 0; 
    int x_register_value = 1; 
    int signal_strength = 0; 
    size_t next_cycle_to_examine = 20; 

    for (const auto& i: instructions) {
        cycle += 1; // either noop or addx add the cycle 

        if (cycle == next_cycle_to_examine) {
            signal_strength += cycle * x_register_value; 
            if ((next_cycle_to_examine += 40) > 220) break; 
        }

        if (i.find("addx") == 0) {
            cycle += 1;     // the second cycle for addx 

            // Recheck if we are in one of the cycles under examination
            // We're still in the beginning of the cycle so the check must be performed before
            // the register is updated
            if (cycle == next_cycle_to_examine) {
                signal_strength += cycle * x_register_value; 
                if ((next_cycle_to_examine += 40) > 220) break; 
            }

            std::string tmp; 
            int register_value_to_add{0}; 
            std::istringstream addx_stream{i}; 
            addx_stream >> tmp; // the "addx" instruction per se  
            addx_stream >> register_value_to_add; 
            x_register_value += register_value_to_add; 
        }
    }

    return signal_strength; 

}

void solve_part_one(const std::vector<std::string>& instructions) {
    auto solution = calculate_signal_strength(instructions); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For part two - Approach: loop across the instruction, and update the 
// column position at each instruction (reset to 0 when we reach the 40th column). 
// If the instruction is addx, the column position is updated twice. If the considered 
// column is in the sprie (included in [x-1, x+1]) then print "#", otherwise print "*"
auto draw_image(const std::vector<std::string>& instructions) {

    const int ROW_WIDTH = 40; 
    const int COLUMN_HEIGHT = 6; 
    int current_row = 0; 
    int current_column = 0; // the column can be see as a cycle counter, together with the row 
    int x_register_value = 1; 
    std::vector<std::string> image(COLUMN_HEIGHT, ""); 

    for (const auto& i: instructions) {
        if (current_row > COLUMN_HEIGHT) break; 

        if (current_column >= x_register_value - 1 && current_column <= x_register_value + 1) {
            image.at(current_row).push_back('#');
        } else {
            image.at(current_row).push_back('.');
        }

        current_column++;
        if (current_column == ROW_WIDTH) {
            current_column = 0; 
            current_row++; 
        }

        if (i.find("addx") == 0) {
            if (current_column >= x_register_value - 1 && current_column <= x_register_value + 1) {
                image.at(current_row).push_back('#');
            } else {
                image.at(current_row).push_back('.');
            }

            current_column++;
            if (current_column == ROW_WIDTH) {
                current_column = 0; 
                current_row++; 
            }
            
            // Update the x register value
            std::string tmp; 
            int x_register_value_change{0}; 
            std::istringstream instruction_stream(i); 
            instruction_stream >> tmp; // "addx" instruction 
            instruction_stream >> x_register_value_change; 
            x_register_value += x_register_value_change; 
        }
    }
    return image;
} 

void solve_part_two(const std::vector<std::string>& instructions) {
    auto solution = draw_image(instructions); 
    std::cout << "The solution to part two is: " << std::endl; 
    for (const auto& s: solution) {
        std::cout << s << std::endl; 
    }
}

