#include <functional>
#include <queue>
#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <utility> 

// The following are the two types of functions that can increase the "item worry level"
// That is, an additive and a multiplicative one 
auto multiplicative_worry_function = [](long int old, long int par) {
    return old * par; 
};

auto additive_worry_function = [](long int old, long int par) {
    return old + par; 
}; 

// This class represents all the monkeys characteristics: 
// * it holds some items, represented by the "worry level", which we containerize in a queue
// * is has a function that calculates the worry level for a specific item 
// * it has a test value that allows deciding to which monkey the item is thrown
// * it has a monkey to which the item is thrown in case of a positive test, and another one for negative tests 
class Monkey {

    private: 
    std::queue<long int> items_;
    std::function<long int(long int, long int)> update_worry_level_;   
    const int number_; 
    const long int test_value_; 
    const int monkey_positive_number_; 
    const int monkey_negative_number_; 
    long int worry_parameter_; 

    public: 
    Monkey(int monkey_number, long int test_value, std::queue<long int> items, 
            int monkey_positive, int monkey_negative) noexcept : 
        number_(monkey_number), test_value_(test_value), items_(std::move(items)), 
            monkey_positive_number_(monkey_positive), monkey_negative_number_(monkey_negative) {
        update_worry_level_ = additive_worry_function; // default: additive function
        worry_parameter_ = -1; // worry_parameter is < 0 if the second parameter is a worry level
    }

    auto get_monkey_number() const noexcept {
        return number_; 
    }

    auto get_test_value() const noexcept {
        return test_value_; 
    }

    void set_worry_function(std::function<long int(long int, long int)> worry_function) noexcept {
        this->update_worry_level_ = worry_function; 
    }

    void set_worry_function_parameter(long int parameter) noexcept {
        this->worry_parameter_ = parameter;
    }

    auto& get_items() noexcept {
        return this->items_;
    }

    // Performs a round for a monkey 
    auto inspect_and_throw_items(long int worry_relief, std::vector<Monkey>& monkeys, bool divideByWorryRelief) -> long int {
        if (items_.size() == 0) {
            return 0; 
        }

        auto& monkey_positive_ = monkeys.at(monkey_positive_number_); 
        auto& monkey_negative_ = monkeys.at(monkey_negative_number_); 

        auto n_items = items_.size(); 
        while (!items_.empty()) {
            auto item_ = items_.front(); 
            long int updated_item; 
            if (this->worry_parameter_ < 0) {
                updated_item = update_worry_level_(item_, item_); 
            } else {
                updated_item = update_worry_level_(item_, this->worry_parameter_); 
            }
            
            // Perform the test and throw the updated item to the corresponding monkey
            // Here, we' re in part one of the assignment
            if (divideByWorryRelief) updated_item /= worry_relief;
            auto test = updated_item % test_value_ == 0;
            // In part two of the assignment, we need to keep the relief manageable
            // So I mod the worry level with the least common multiple of 
            // all the divisor test values (also passed as worry relief here)
            if (!divideByWorryRelief) updated_item %= worry_relief;  
            if (test) {
                monkey_positive_.get_items().push(updated_item); 
            } else {    
                monkey_negative_.get_items().push(updated_item); 
            }
            items_.pop(); 
        }
        return n_items; 
    }

    void to_string() const {
        // Quite expensive function as we are copying the std::queue 
        // Used only for testing purposes 
        std::queue<long int> items_copy = this->items_; 
        std::cout << "Monkey " << number_ << ":" << std::endl; 
        std::cout << "  " << "Starting items: "; 
        while (!items_copy.empty()) {
            std::cout << items_copy.front() << " "; 
            items_copy.pop(); 
        }
        std::cout << std::endl; 
        std::cout << "Parameter: " << worry_parameter_ << std::endl; 
        std::cout << "Monkey if true: " << monkey_positive_number_ << std::endl;
        std::cout << "Monkey if false: " << monkey_negative_number_ << std::endl;
    }
};

void get_data(const std::string&, std::vector<Monkey>&); 
void solve_part_one(std::vector<Monkey>); 
void solve_part_two(std::vector<Monkey>); 


int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 11 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<Monkey> monkeys; 

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, monkeys); 

    if (is_test) {
        for (const auto& m: monkeys) {
            m.to_string(); 
        }
    }

    solve_part_one(monkeys); 
    solve_part_two(monkeys); 

    return 0; 
}

void get_data(const std::string& file_name, std::vector<Monkey>& monkeys) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        // This map keeps track of the value monkeys that will receive the items from the key monkey
        // It will be used after the file is read to assign the monkeys with the corresponding
        // class function
        std::unordered_map<long int, std::pair<long int, long int>> tmp_monkeys_mapping; 
        std::queue<long int> monkey_items; 
        bool is_multiplicative{false}; // establishes whether the worry function is additive (false) or multiplicative (true)
        int monkey_number; 
        long int test_value, parameter_value = -1; 
        long int pos_monkey_number, neg_monkey_number; 

        while (std::getline(input_file, line)) {
            if (line.empty()) continue; 
            std::istringstream line_stream{line}; 
            std::string tmp_data; 

            if (line.find("Monkey") == 0) {
                line_stream >> tmp_data; 
                line_stream >> monkey_number;  
                continue; 
            } else if (line.find("Starting") == 2) {
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                long int tmp_item_value;
                while(line_stream >> tmp_item_value) { 
                    monkey_items.push(tmp_item_value); 
                    line_stream >> tmp_data; 
                }
                continue; 
            } else if (line.find("Operation") == 2) {
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                is_multiplicative = (tmp_data == "*"); 
                line_stream >> tmp_data; 
                if (tmp_data != "old") parameter_value = std::stoi(tmp_data); 
                continue; 
            } else if (line.find("Test") == 2) {
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                line_stream >> tmp_data; 
                line_stream >> test_value; 
                continue; 
            } else if (line.find("If true") == 4) {
                for (size_t i = 0; i < 5; ++i) line_stream >> tmp_data; 
                line_stream >> pos_monkey_number;
                continue; 
            } else if (line.find("If false") == 4) {
                for (size_t i = 0; i < 5; ++i) line_stream >> tmp_data; 
                line_stream >> neg_monkey_number;

                // Create the monkey and reset quantities for the next monkey
                Monkey monkey = Monkey(monkey_number, test_value, std::move(monkey_items), pos_monkey_number, neg_monkey_number); 
                if (is_multiplicative) monkey.set_worry_function(multiplicative_worry_function); 
                if (parameter_value >= 0) monkey.set_worry_function_parameter(parameter_value); 
                monkeys.push_back(monkey); 

                while (!monkey_items.empty()) monkey_items.pop(); 
                parameter_value = -1; 
            }
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

} 

// Approach: given the number of rounds and a specific worry relief as an input, 
// plays multiple rounds of keep away by calling the inpspect_and_throw_items function 
// of the monkey class. A vector of the same size of the monkeys keeps track of the 
// number of items each monkey inspects. 
// For part 2: to avoid too large level of worries, the worry relief is calculated by 
// multiplying all the monkey divisors, which will be handled as a modulo operator in 
// the inspect_and_throw_items function.
auto play_keep_away(std::vector<Monkey> monkeys, long int worry_relief, size_t n_rounds) {

    std::vector<size_t> n_ispected_items(monkeys.size(), 0);
    bool divideByWorryLevel = true; 

    // If there's no worry relief, we need to find an alternative way to manage worry levels - 
    // So we find the least common multiple of all test values and use that as mod operator
    // for the items' worry levels. Note that we are dealing with only prime numbers, 
    // so we take the multiplication of such values as LCM
    if (worry_relief == 0) {
        long int tmp{1}; 
        for (auto& m: monkeys) {
            tmp *= m.get_test_value(); 
        }
        worry_relief = tmp; 
        divideByWorryLevel = false; 
    }

    for (size_t i =0; i < n_rounds; ++i) {
        for (size_t k = 0; k < monkeys.size(); k++) {
            n_ispected_items.at(k) += monkeys.at(k).inspect_and_throw_items(worry_relief, monkeys, divideByWorryLevel); 
        }
    }

    std::partial_sort(n_ispected_items.begin(), n_ispected_items.begin() + 2, 
        n_ispected_items.end(), std::greater<long int>());
    auto largest1 = n_ispected_items[0];
    auto largest2 = n_ispected_items[1];
    return largest1 * largest2; 

}

// Note that we have to pass the vector by value (rather than by reference)
// as we need a fresh copy for part 2 
void solve_part_one(std::vector<Monkey> monkeys) {
    auto solution = play_keep_away(monkeys, 3, 20); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

// For part two we need to remove the worry level and increase the rounds to 10000
void solve_part_two(std::vector<Monkey> monkeys) {
    auto solution = play_keep_away(monkeys, 0, 10000); 
    std::cout << "The solution to part two is " << solution << std::endl; 
}


