#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

/* Finally, the last day of AoC 2022! In today's puzzle we are required to transform  
 * some SNAFU number into decimal, perform their sum, and return the sum converted back to SNAFU.
 * SNAFU is a sort of base-5 numbering which allows for negative (-2 and -1) digits, and therefore it 
 * each position ranges between -2 (=) and +2. Each position increases its value by 5: 1 = 1, 10 = 5, 100 = 25, and so on.
 * To perform the conversion from SNAFU to digits: I will iterate through each SNAFU digit one by one, converting it into
 * decimal based on the position it occupies in the reference SNAFU numbers (so that = will become -2, - is -1, etc.); by 
 * adding such number to the current running total (multiplyed by 5 at each round) we will obtain the final decimal digit.  
 * To perform the inverse conversion: this is a bit trickier. What I did was to calculate the remainder of the decimal 
 * number division by 5, and dividing the number itself by 5. If the remainder is <= 2, I just pushed it in front of the 
 * current SNAFU number. Else, if the remainder is 3 or 4, it means that we need to subtract either 2 or 1 from the next decimal 
 * digit, and at the same time also incrementing the decimal number by one to keep track of the subtraction operation. This 
 * will be then pushed in front of the current SNAFU number. As an alternative, I prepared also a recursive solution that 
 * performs the same operation recursively. 
 * 
 * And that's it! There is no part 2 puzzle for today. I must say I had fun working on the AoC 2022 solutions in c++. I 
 * learned I lot of stuff and gave me lots of insights. 
 */

void get_data(const std::string&, std::vector<std::string>&);  
void solve_part_one(const std::vector<std::string>&); 
void solve_part_two(); 

int main(int argc, char* argv[]) {
  
  std::cout << ">>> Advent Of Code 2022 - Day 25 <<<" << std::endl; 
  
  std::string file_name; 
  bool is_test = false; 
  std::vector<std::string> snafu_numbers;
  
  if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
  if (is_test) file_name = "./test.txt"; 
  else file_name = "./input.txt"; 
  
  get_data(file_name, snafu_numbers); 
  
  if (is_test) {
    for (const auto& n: snafu_numbers) std::cout << n << std::endl; 
  }
  
  solve_part_one(snafu_numbers); 
  solve_part_two(); 
  
  return 0; 
}


// Get the input data
void get_data(const std::string& file_name, std::vector<std::string>& snafu_numbers) {
  
  std::ifstream input_file{file_name}; 
  
  if (input_file.is_open()) {
    std::string line; 
    while (std::getline(input_file, line)) {
      snafu_numbers.push_back(line); 
    }
  } else {
    std::cerr << "Could not open the file." << std::endl; 
    exit(0); 
  }
  input_file.close(); 
} 

// Algorithms
// Converts a snafu number to decimals, given the string of snafu symbols ([=-012])
auto snafu_to_decimal(const std::string& snafu_number, const std::string& snafu_symbols) -> long int {
  long int dec_number = 0; 
  for (const auto& c: snafu_number) {
    auto dec_char = snafu_symbols.find(c);
    dec_number = dec_number * 5 + dec_char - 2; 
  }
  return dec_number; 
}

// Stable modulo, useful in case of negative numbers 
// auto stable_modulo(long int a, long int b) -> long int {
//   return a >= 0 ? a % b : (b - std::abs(a % b)) % b;
// }

// Converts a decimal number back to snafu. To do that, it checks if we need 
// to subtract one or two from the next digit (= or -) in which case we also increase
// the value of the next decimal digit.
auto decimal_to_snafu(long int dec_number) -> std::string {
  
  const std::string snafu_chars = "=-"; 
  std::string snafu_value{""}; 
  while (dec_number != 0) {
    
    auto rem = dec_number % 5; 
    dec_number /= 5; 
    if (rem <= 2) snafu_value = std::to_string(rem) + snafu_value; 
    else {
      snafu_value = snafu_chars.at(rem-3) + snafu_value;
      dec_number += 1; 
    }
  }  
  return snafu_value; 
  
}

// This is a recursive solution for the back-conversion. It works exactly like the previous function
// auto decimal_to_snafu_recursive(long int dec_number) -> std::string {
//   if (dec_number == 0) return ""; 
//   std::string snafu_chars = "012=-"; 
//   
//   auto rem = dec_number % 5; 
//   std::string snafu_value = std::string(1, snafu_chars.at(rem)); 
//   auto remaining_snafu = decimal_to_snafu_recursive((dec_number + 2) / 5); // + 2 because snafu digits start at -2
//   return remaining_snafu + snafu_value; 
//   
// }

// Converts all numbers in the SNAFU list into decimal, it sums them, and finally it 
// converts the sum back to its snafu counterpart
auto convert_and_sum(const std::vector<std::string>& snafu_numbers) -> std::string {
  long int sum = 0; 
  std::string snafu_symbols = "=-012";
  for (const auto& s: snafu_numbers) {
    sum += snafu_to_decimal(s, snafu_symbols); 
  }
  return decimal_to_snafu(sum); 
  // return decimal_to_snafu_recursive(sum); 
  
}

void solve_part_one(const std::vector<std::string>& snafu_numbers) {
  auto solution = convert_and_sum(snafu_numbers);
  std::cout << "The solution to part one is " << solution << std::endl;
}

void solve_part_two() {
  std::cout << "             >>>>> The end <<<<<" << std::endl;
}
