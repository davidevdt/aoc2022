#include <string>
#include <iostream>
#include <cstdint>
#include <stdlib.h>
#include <sstream> 
#include <fstream> 
#include <vector> 

void get_data(const std::string&, std::vector<std::string>&); 

int main(int argc, char** argv) {

    std::string file_name; 
    bool is_test = false; 
    std::vector<std::string> datastreams; // A vector ain't really necessary here, but the test consists of several strings

    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 

    if (is_test) {
        file_name = "./test.txt"; 
        datastreams.reserve(4); // 4 test cases
    }
    else {
        file_name = "./input.txt"; 
        datastreams.reserve(1); 
    }

    get_data(file_name, datastreams); 

    if (is_test) {
        for (const auto& d: datastreams) {
            std::cout << d << std::endl; 
        }
    }


    return 0; 
}

void get_data(const std::string& file_name, std::vector<std::string>& datastreams) {

    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string datastream; 

        while (std::getline(input_file, datastream)) {
            datastreams.push_back(datastream);
        } 
    } 
    else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }

    input_file.close(); 

}