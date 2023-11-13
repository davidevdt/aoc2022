#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <unordered_map>

// To solve this day's puzzle, I will use a recursive methodology. I will try through recursive calls to the 
// number calculating function to reconstruct the number that the root monkey is supposed to yell. To perform the 
// operations, I will use a custom-defined function that takes the operation character ('-', '+', ...) as input, 
// along with the two other input numbers, and performs the corresponding operation. The monkeys with a number and the 
// monkeys whose number needs to be calculated are stored in their own struct. Both types of monkeys will be stored in 
// two different hashmaps, one for each type of monkeys. 
// For part 2, I will still use the concept of recursion to perform two operations: (a) finding out through the monkeys' tree
// which monkey depends on the human yell; and (b) finding the number that humn needs to call in order to equate the 
// root's two numbers. This solution is inspired by the Rust's solution given in: 
// https://nickymeuleman.netlify.app/garden/aoc2022-day21

// Given a character operation as input and two numbers, it performs the corresponding operation 
auto perform_monkey_operation(const char& op, long int first, long int second) -> long int {
    switch(op) {
        case '+': 
            return first + second; 
        case '-':
            return first - second; 
        case '*':
            return first * second; 
        case '/': 
            return static_cast<long int>(static_cast<double>(first) / second); 
        default:
            return static_cast<long int>(0); // just a placeholder for the default case 
    }
}

// Monkeys with known number
struct NumberMonkey {
    long int n; 
    NumberMonkey() : n(0) {}
    NumberMonkey(long int n_) : n(n_) {}

    auto print() const {std::cout << n << std::endl;}
}; 

// Monkey whose number needs to be calculated
struct OpMonkey {
    char op; 
    std::string monkey_one; 
    std::string monkey_two; 
    OpMonkey() : monkey_one(""), monkey_two(""), op(' ') {}
    OpMonkey(std::string m_one_, char op_, std::string m_two_): monkey_one(m_one_), op(op_), monkey_two(m_two_) {}

    auto print() const {std::cout << monkey_one << op << monkey_two << std::endl;}
}; 

void get_data(const std::string&, std::unordered_map<std::string, NumberMonkey>&, std::unordered_map<std::string, OpMonkey>&);  
void solve_part_one(const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys); 
void solve_part_two(const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 21 <<<" << std::endl; 

    std::string file_name; 
    bool is_test = false; 
    std::unordered_map<std::string, NumberMonkey> number_monkeys; 
    std::unordered_map<std::string, OpMonkey> op_monkeys; 
    
    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, number_monkeys, op_monkeys); 

    if (is_test) { 
        for (const auto& item: number_monkeys) {
            std::cout << item.first << ":\t";
            item.second.print(); 
        }

        for (const auto& item: op_monkeys) {
            std::cout << item.first << ":\t";
            item.second.print(); 
        }
    }

    solve_part_one(number_monkeys, op_monkeys); 
    solve_part_two(number_monkeys, op_monkeys); 

    return 0; 
}

// Get the input data
void get_data(const std::string& file_name, std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                std::unordered_map<std::string, OpMonkey>& op_monkeys) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            std::istringstream line_stream(line); 
            std::string monkey_name; 

            std::getline(line_stream, monkey_name, ':'); 
            line_stream.ignore(); // ignore the next white space              
            std::string rest_of_line;
            std::getline(line_stream, rest_of_line); 
            if (std::isdigit(rest_of_line.at(0))) {
                long int n = std::stol(rest_of_line); 
                number_monkeys.emplace(monkey_name, NumberMonkey(n)); 
            } else {
                std::string monkey_one, monkey_two; 
                char op; 
                std::istringstream monkey_stream(rest_of_line); 
                monkey_stream >> monkey_one; 
                monkey_stream >> op; 
                monkey_stream >>  monkey_two; 
                op_monkeys.emplace(monkey_name, OpMonkey(monkey_one, op, monkey_two)); 
            }
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// Recursion calculation of the monkey's numbers, the operation is performed only once both numbers are performed
auto calculate_monkey_number(const std::string monkey_name, const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                                    const std::unordered_map<std::string, OpMonkey>& op_monkeys) {

    if (number_monkeys.find(monkey_name) != number_monkeys.end()) {
        return number_monkeys.find(monkey_name)->second.n; 
    }

    auto monkey_one = op_monkeys.find(monkey_name)->second.monkey_one; 
    auto monkey_two = op_monkeys.find(monkey_name)->second.monkey_two; 
    auto op = op_monkeys.find(monkey_name)->second.op; 

    auto monkey_one_number = calculate_monkey_number(monkey_one, number_monkeys, op_monkeys); 
    auto monkey_two_number = calculate_monkey_number(monkey_two, number_monkeys, op_monkeys); 

    return perform_monkey_operation(op, monkey_one_number, monkey_two_number); 
}

void solve_part_one(const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys) {
    auto solution = calculate_monkey_number("root", number_monkeys, op_monkeys);
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For part two: find out whether for a specific monkey's path depend on the humn or not (returns a boolean)
// Of course, for monkeys whose number is already known there is no such dependency
auto is_humn_in_path(const std::string& monkey_name, const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys) {

    if (monkey_name == "humn") return true; 
    if (number_monkeys.find(monkey_name) != number_monkeys.end()) return false; 

    auto monkey_one = op_monkeys.find(monkey_name)->second.monkey_one; 
    auto monkey_two = op_monkeys.find(monkey_name)->second.monkey_two; 
    return is_humn_in_path(monkey_one, number_monkeys, op_monkeys) || is_humn_in_path(monkey_two, number_monkeys, op_monkeys); 
}

// Given a value to equate and a monkey number as input, it performs the inverse operation (e.g. x = a + b -> a = x - b)
// to find out the number that needs to be equated. The recursion ends when we encounter the humn (ourself)
auto calculate_humn_equality(const std::string monkey_name, long int equating_value, 
                             const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                             const std::unordered_map<std::string, OpMonkey>& op_monkeys) {

    if (monkey_name == "humn") return equating_value; 

    if (number_monkeys.find(monkey_name) != number_monkeys.end()) {
        return number_monkeys.find(monkey_name)->second.n; 
    }

    auto monkey_one = op_monkeys.find(monkey_name)->second.monkey_one; 
    auto monkey_two = op_monkeys.find(monkey_name)->second.monkey_two; 
    auto op = op_monkeys.find(monkey_name)->second.op; 

    if (is_humn_in_path(monkey_one, number_monkeys, op_monkeys)) {

        char symmetric_op; 
        switch (op) {
            case '+':
                symmetric_op = '-'; 
                break; 
            case '-':
                symmetric_op = '+'; 
                break; 
            case '*':
                symmetric_op = '/'; 
                break; 
            default: 
                symmetric_op = '*'; 
                break; 
        }

        auto monkey_two_number = calculate_monkey_number(monkey_two, number_monkeys, op_monkeys); 
        auto new_equating_value = perform_monkey_operation(symmetric_op, equating_value, monkey_two_number); 
        return calculate_humn_equality(monkey_one, new_equating_value, number_monkeys, op_monkeys);
    } else {

        auto monkey_one_number = calculate_monkey_number(monkey_one, number_monkeys, op_monkeys); 

        char symmetric_op; 
        long int lhs, rhs; 

        switch (op) {
            case '+':
                symmetric_op = '-'; 
                lhs = equating_value; 
                rhs = monkey_one_number; 
                break; 
            case '-':
                symmetric_op = '-';  // E.g. Suppose 600 = 4 - lgvd -> 600 - 4 = -lgvd -> lgvd = 4 - 600 
                lhs = monkey_one_number; 
                rhs = equating_value; 
                break; 
            case '*':
                symmetric_op = '/'; 
                lhs = equating_value; 
                rhs = monkey_one_number; 
                break; 
            default: 
                symmetric_op = '/'; 
                lhs = monkey_one_number; 
                rhs = equating_value; 
                break; 
        }

        auto new_equating_value = perform_monkey_operation(symmetric_op, lhs, rhs); 
        return calculate_humn_equality(monkey_two, new_equating_value, number_monkeys, op_monkeys);   
    }
}

// Starts the recursion for part two by finding out which number the root monkey needs to equate
auto run_part_two(const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys) {

    auto root_monkey = op_monkeys.find("root"); 

    if (is_humn_in_path(root_monkey->second.monkey_one, number_monkeys, op_monkeys)) {
        auto equating_value = calculate_monkey_number(root_monkey->second.monkey_two, number_monkeys, op_monkeys);  
        return calculate_humn_equality(root_monkey->second.monkey_one,  equating_value, number_monkeys, op_monkeys); 
    } else {
        auto equating_value = calculate_monkey_number(root_monkey->second.monkey_one, number_monkeys, op_monkeys); 
        return calculate_humn_equality(root_monkey->second.monkey_two,  equating_value, number_monkeys, op_monkeys);        
    }

}

void solve_part_two(const std::unordered_map<std::string, NumberMonkey>& number_monkeys, 
                    const std::unordered_map<std::string, OpMonkey>& op_monkeys) {
    auto solution = run_part_two(number_monkeys, op_monkeys);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


