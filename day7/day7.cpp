#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector> 
#include <memory> 
#include <sstream> 
#include <map> 

using file = std::pair<size_t, std::string>; 

// The following class aims at representing a pc's folders. 
class folder {
	folder* parent = nullptr; 
	std::vector<std::unique_ptr<folder>> subfolders; 
	std::vector<file> files; 

	public: 
	std::string name;
	folder() = default; 

	folder(std::string&& n) : name(std::move(n)) {}

	auto get_parent() {
		if (this->name == "/") return this; 
		return this->parent; 
	}

	void set_parent(folder* parent_folder) {
		if (this->name == "/") return; // "/" cannot have a parent 
		if (parent == nullptr) this->parent = parent_folder;
	}
	
	void add_subfolder(std::unique_ptr<folder>& sub_folder) {
		this->subfolders.push_back(std::move(sub_folder)); 
	}
	
	// void add_subfolder(std::string& sub_folder_name) {
	// 	this->subfolders.emplace_back(std::make_unique<folder>(std::move(sub_folder_name))); 
	// }

	const std::vector<folder*> get_subfolders() const {
		std::vector<folder*> result;
		for (const auto& subfolder : subfolders) {
			result.push_back(subfolder.get());
		}
		return result;
	}

	void add_file(const size_t size, std::string& file_name) {
		this->files.emplace_back(file{size, file_name}); 
	}

	std::vector<file> get_files() const {
		return this->files; 
	}

	folder* find_subfolder(const std::string& sub_folder_name) const {
		for (auto& s: this->subfolders) {
			if (s->name == sub_folder_name) {
				return s.get();  // equivalent: return &*s;  
				// If return type is folder& : return *s; 
			}
		}
		return nullptr; 
	}

	std::string scan_folder(unsigned int level=0) const {
		std::ostringstream folder_scanner{""};  
		auto prefix = std::string(level * 2, ' ') + (level > 0 ?  "- " : "");  
		folder_scanner << prefix  << this->name << " (dir)\n"; 
		if (this->subfolders.size() > 0) {
			for (const auto& s: subfolders) {
				folder_scanner << s->scan_folder(level + 1); 
			}
		}
		if (this->files.size() > 0) {
			auto file_prefix = std::string(level + 1, ' ') + (level == 0 ? " - " : prefix); 
			for (const auto& f: files) {
				folder_scanner << file_prefix << f.second << " (file, " << f.first << ")" "\n";  
			}
		}
		return folder_scanner.str();  
	}
}; 

folder get_data(const std::string&); 
void solve_part_one(const folder&); 
void solve_part_two(const folder&); 

int main(int argc, char* argv[]) {
	std::cout << ">>> Advent Of Code 2022 - Day 7 <<<" << std::endl; 

    std::string file_name; 
    bool is_test = false;  

	if (argc > 1 && std::string(argv[1]) == "test") {is_test = true;}

	if (is_test) file_name = "./test.txt"; 
	else file_name = "./input.txt";

	auto file_system = get_data(file_name); 
	if (is_test) {
		std::cout << file_system.scan_folder() << std::endl; 
	}

	solve_part_one(file_system); 
    solve_part_two(file_system); 
}

folder get_data(const std::string& file_name) {

	std::ifstream input_file{file_name};  
	folder file_system("/"); 

	if (input_file.is_open()) {
		std::string line; 
		folder* current_folder = &file_system; 

		while(std::getline(input_file, line)) {
			std::istringstream line_stream(line); 
			std::string next_token; 
			line_stream >> next_token; 
			if (next_token == "$") {
				line_stream >> next_token; 
				if (next_token == "cd") {
					line_stream >> next_token; 
					if (next_token == "..") {
						current_folder = current_folder->get_parent(); 
					}
					else {
						line_stream >> next_token; // subfolder name after "cd"
						auto tmp_folder = current_folder->find_subfolder(next_token);
						if (tmp_folder) current_folder = tmp_folder; 
					}
				} else { // ls 
					continue; 
				}
			} else if (next_token == "dir") {
				line_stream >> next_token; 
				std::unique_ptr<folder> new_folder = std::make_unique<folder>(std::move(next_token)); 
				new_folder->set_parent(current_folder); 
				current_folder->add_subfolder(new_folder);
			} else {
				size_t file_size = std::stoul(next_token); 
				line_stream >> next_token; // file name
				current_folder->add_file(file_size, next_token); 
			}
		}

	} else {
		std::cerr << "Could not open the file." << std::endl; 
		exit(0); 
	}

	input_file.close(); 
	return file_system; 
} 



size_t calculate_folder_size(const folder& folder, std::multimap<size_t, std::string>& folders_size) {
	size_t size = 0; 

	auto subfolders = folder.get_subfolders(); 
	if (subfolders.size() > 0) {
		for (const auto& s: subfolders) {
			size += calculate_folder_size(*s, folders_size); 
		}	
	}

	auto files = folder.get_files(); 
	if (files.size() > 0) {
		for (const auto& f: files) {
			size += f.first; 
		}
	}

	folders_size.insert({size, folder.name}); 
	return size;
}

// Approach: store the folders sizes in a ordered multimap. After that, 
// loop through the map until the folders reach size 100000. Sum up the 
// sizes of such folders and report the result. 
auto calculate_size_deletable_folders(const folder& file_system) {
	size_t result; 
	std::multimap<size_t, std::string> folders_size; 
	calculate_folder_size(file_system, folders_size); 

	size_t folder_size = 0; 
	auto folders_iterator = folders_size.begin(); 
	while (folders_iterator != folders_size.end()) {
		if (folders_iterator->first > 100000) break; 
		folder_size += folders_iterator->first; 
		folders_iterator++; 
	}

	return folder_size; 
}

void solve_part_one(const folder& file_system) {
	auto solution = calculate_size_deletable_folders(file_system); 
	std::cout << "The solution to part one is " << solution << std::endl; 
} 

// Find the smallest one among the candidate folders that can free up 
// enough space on disk if removed. Once again, use the ordered 
// multimap to store the folders sizes. 
auto calculate_size_smallest_large_folder(const folder& file_system) {
	size_t result{0}; 
	std::multimap<size_t, std::string> folders_size; 
	auto unused_space = 70000000 - calculate_folder_size(file_system, folders_size); 

	for (const auto& f: folders_size) {
		if (unused_space + f.first >= 30000000) {
			result = f.first; 
			break; 
		}
	}

	return result; 
}

void solve_part_two(const folder& file_system) {
	auto solution = calculate_size_smallest_large_folder(file_system); 
	std::cout << "The solution to part two is " << solution << std::endl; 
} 