#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 

// To solve this level I will store the numbers as Nodes of a doubly linked list. A vector of such nodes will be useful 
// to preserve the original order of the numbers, and to create the right original references between previous and next
// pointers to the current node. When the list of numbers is mixed, then the pointers are updated accordingly. The 
// circularity of the list is ensured by using the modulo operator when indexing the lists elements. 
// The solution is inspired by the Python solution given in: 
// https://github.com/hyper-neutrino/advent-of-code/blob/main/2022/day20p1.py

struct Node {

    long int val; 
    Node* next = nullptr; 
    Node* prev = nullptr;
    explicit Node (long int v): val(v) {} 
    Node() {}
    // Node (const Node& other): val(other.val), next(nullptr), prev(nullptr) {}    // -> this will destroy references when extracting a node from the list! 
}; 

// This will set the correct references to the list of nodes 
// So that we can create the circular dependencies in the list 
auto set_vector_pointers(std::vector<Node>& list) {
    for (size_t i = 0; i < list.size(); ++i) {
        list.at(i).next = &list.at((i+1) % list.size()); 
        list.at(i).prev = (i == 0) ? &list.at(list.size()-1) : &list.at((i-1) % list.size()); 
    }
}

void get_data(const std::string&, std::vector<Node>&);  
void solve_part_one(std::vector<Node>&); 
void solve_part_two(std::vector<Node>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 20 <<<" << std::endl; 

    std::string file_name; 
    bool is_test = false; 
    std::vector<Node> list; 
    
    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) file_name = "./test.txt"; 
    else file_name = "./input.txt"; 

    get_data(file_name, list); 

    if (is_test) { 
        Node node = list.at(0); 
        size_t i = 0; 
        while (i < list.size()) {
            std::cout << node.val; 
            if (i < list.size() - 1) std::cout << " -> "; 
            else std::cout << std::endl; 
            node = *node.next;   
            // node = *node.prev;   
            ++i; 
        }
    }

    solve_part_one(list); 
    set_vector_pointers(list); // For part two we need to reset the list (as we are changing the list references)
    solve_part_two(list); 

    return 0; 
}

// Get the input data
void get_data(const std::string& file_name, std::vector<Node>& list) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        while (std::getline(input_file, line)) {
            long int new_val = std::stol(line); 
            list.emplace_back(Node(new_val)); 
        }
        set_vector_pointers(list); 
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// Mix the decription list of numbers. The decryption_key and n_mix parameters are used in part two. 
auto decrypt_list(std::vector<Node>& list, long int decryption_key = 1, size_t n_mix = 1) {

    if (decryption_key != 1) {
        for (auto& node: list) {
            node.val *= decryption_key; 
        }
    }

    Node* starting_node; 

    for (size_t i = 0; i < n_mix; ++i) {

        // Loop through the numbers' original list ordering
        for (auto& node: list) {
            
            // Find the 0-value node to be used for the start the counters for the final decryption 
            if (node.val == 0) {starting_node = &node; continue;} // no need to move the 0-value node 

            // Find how much you need to move the nodes and update old and new references 
            // We need to distinguish between negative and positive moves as when we move left we 
            // move one position extra in absolutve value (e.g. +2: moves from 0 to 2; -2: moves from 0 to -3)
            auto* moved_node = &node; 
            if (node.val > 0) {
                // To ensure circularty check the next move by an amount divided by the list's size 
                // with the modulus operator 
                for (size_t tmp = 0; tmp < (node.val % (list.size()-1)); ++tmp) {
                    moved_node = moved_node->next; 
                }
                if (moved_node == &node) continue; // We are back at the same node, no need to move anything
                node.next->prev = node.prev; 
                node.prev->next = node.next; 
                moved_node->next->prev = &node; 
                node.next = moved_node->next;
                moved_node->next = &node; 
                node.prev = moved_node;   
            } 
            else {
                for (size_t tmp = 0; tmp < ((-node.val) % (list.size()-1)); ++tmp) {
                    moved_node = moved_node->prev; 
                }
                if (moved_node == &node) continue; // We are back at the same node, no need to move anything
                node.prev->next = node.next; 
                node.next->prev = node.prev; 
                moved_node->prev->next = &node; 
                node.prev = moved_node->prev;
                moved_node->prev = &node; 
                node.next = moved_node;   
            }
        }

        // For debugging
        // size_t ii = 0; 
        // Node v = *starting_node; 
        // std::cout << "round " << i << std::endl; 
        // while (ii < list.size()) {
        //     std::cout << v.val << " "; 
        //     v = *v.next; 
        //     ++ii; 
        // }
        // std::cout << std::endl; 
    }

    // Now count the final grove coordinate...
    long int grove_coordinate = 0; 
    for (size_t it1 = 0; it1 < 3; ++it1) {
        for (size_t it2 = 0; it2 < 1000; ++it2) {
            starting_node = starting_node->next; 
        }
        // std::cout << starting_node->val << std::endl; 
        grove_coordinate += starting_node->val; 
    }
    return grove_coordinate; 
}


void solve_part_one(std::vector<Node>& list) {
    auto solution = decrypt_list(list);
    std::cout << "The solution to part one is " << solution << std::endl; 
}

void solve_part_two(std::vector<Node>& list) {
    auto solution = decrypt_list(list, 811589153, 10);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


