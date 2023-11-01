#include <iostream> 
#include <string>
#include <sstream>
#include <fstream> 
#include <vector> 
#include <utility> 
#include <algorithm>
#include <cmath>
#include <regex>  
#include <unordered_map> 
#include <unordered_set> 
#include <limits> 

// Approach: rather than looping through numbers, I will use ranges (specified in the Interval struct) to 
// solve today's puzzle. This will definitely improve the solution in term of efficiency. 
// Each Sensor will be stored in its own struct class, which contains the sensor's coordinate, the closes beacon 
// coordinate, and their Manatthan distance. The core algorithm will take a line number and a sensor as an input, and 
// will return an Interval (specified by two numbers: inferior and superior extremes of the interval) which represent 
// the range of the non-empty coordinates at that line number, according to the sensor's manatthan distance from the beacon. 


using Coordinate = std::pair<long int, long int>; 

auto calculate_manatthan_distance = [](long int& x1, long int& y1, long int& x2, long int& y2) -> long int {
    return std::abs(x1 - x2) + std::abs(y1 - y2); 
};

struct Sensor {

    const Coordinate coordinate; 
    const Coordinate closest_beacon; 
    const long int distance; 

    explicit Sensor(long int sx, long int sy, long int bx, long int by): coordinate(sx, sy), closest_beacon(bx, by), 
        distance(calculate_manatthan_distance(sx, sy, bx, by)) { 
    }

    auto print() const noexcept{
        std::string s_string= "Sensor: (" + std::to_string(coordinate.first) + ',' + std::to_string(coordinate.second) + 
            ") -> Beacon: (" + std::to_string(closest_beacon.first) + ',' + std::to_string(closest_beacon.second) + ')' + 
            ":::Manatthan Distance: " + std::to_string(distance) ;
        std::cout << s_string << std::endl;
    }
}; 

struct Interval {
    long int inf, sup; 

    enum class OVERLAP_TYPE {
        NO_OVERLAP, 
        FULLY_CONTAINED, 
        FULLY_CONTAINS, 
        LEFT_OVERLAP, 
        RIGHT_OVERLAP
    }; 

    // Constructor 
    explicit Interval(long int i, long int s) noexcept: inf(i), sup(s) {
        if (inf > sup) {
            long int tmp = inf; 
            inf = sup; 
            sup = inf; 
        }
    }

    // Checks if an interval is overlapped by another 
    auto overlaps(const Interval& other) {
        if (this->inf > other.sup || this->sup < other.inf) return OVERLAP_TYPE::NO_OVERLAP;
        if (this->inf > other.inf && this->sup < other.sup)  return OVERLAP_TYPE::FULLY_CONTAINED; 
        if (this->inf < other.inf && this->sup > other.sup) return OVERLAP_TYPE::FULLY_CONTAINS; 
        if (this->inf < other.inf && this->sup < other.sup) return OVERLAP_TYPE::LEFT_OVERLAP; 
        // if (this->inf > other.inf && this->sup > other.sup) return OVERLAP_TYPE::LEFT_OVERLAP; 
        return OVERLAP_TYPE::LEFT_OVERLAP; 
    }

    // Given another interval as an input, extends th12518502636475e current interval accordingly
    // if there is some overlap: 
    // (a) if the other interval is larger on both extremes, the other interval's extremes will replace the current one's
    // (b) if the overlap only occurs in one of the two extremes, the extreme is updated accordingly
    // (c) if the current Interval is larger in both extremes, no update is performed 
    // (d) if there's no overlap, nothing is done and the function returns false 
    auto extend(const Interval& other) {
        auto overlap_type = this->overlaps(other);
        if (overlap_type == OVERLAP_TYPE::NO_OVERLAP) return false; 

        if (overlap_type == OVERLAP_TYPE::FULLY_CONTAINED) {
            this->inf = other.inf; this->sup = other.sup; 
        }
        else if (overlap_type == OVERLAP_TYPE::FULLY_CONTAINS) {
            // DO NOTHING    
        }
        else if (overlap_type == OVERLAP_TYPE::LEFT_OVERLAP) {
            this->sup = other.sup; 
        }
        else if (overlap_type == OVERLAP_TYPE::RIGHT_OVERLAP) {
            this->inf = other.inf; 
        }
        return true; 
    }

    auto print () const {
        std::cout << "[" << std::to_string(inf) << "," << std::to_string(sup) << "]" << std::endl; 
    }

    auto size() const {
        return std::abs(sup - inf) + 1; 
    }

    // Checks if an input number is contained in the interval 
    auto contains(long int x) const {
        return x >= inf && x <= sup; 
    }

    // Compares two intervals: 
    // if the current interval's inf is lower than the input one's, returns true 
    // if the two intervals have the same inf and this interval's sup is lower than the input one's, returns true
    // else it returns false 
    bool operator<(const Interval& other) const {
        if (this->inf < other.inf) return true; 
        else if (this-> inf == other.inf) return this->sup < other.sup; 
        else return false; 
    }

};


void get_data(const std::string&, std::vector<Sensor>&);  
void solve_part_one(const std::vector<Sensor>&, long int); 
void solve_part_two(std::vector<Sensor>&, const std::vector<long int>&); 

int main(int argc, char* argv[]) {

    std::cout << ">>> Advent Of Code 2022 - Day 15 <<<" << std::endl; 

    std::string file_name; 
    bool is_test; 
    std::vector<Sensor> sensors; 
    long int row_nr = 2000000; 


    if (argc > 1 && std::string(argv[1]) == "test") is_test = true; 
    if (is_test) {file_name = "./test.txt"; row_nr = 10;} 
    else file_name = "./input.txt"; 

    get_data(file_name, sensors); 

    if (is_test) {
        for (const auto& s: sensors) s.print(); 
    }

    solve_part_one(sensors, row_nr); 

    std::vector<long int> extremes{0, 4000000, 0, 4000000};
    if (is_test) {
        extremes = {0, 20, 0, 20};
    } 
    solve_part_two(sensors, extremes); 

    return 0; 
}

// Just for a change, the data will be imported using regular expression pattern matching 
void get_data(const std::string& file_name, std::vector<Sensor>& sensors) {
    std::ifstream input_file{file_name}; 

    if (input_file.is_open()) {
        std::string line; 
        std::regex pattern("Sensor at x=(-?\\d+), y=(-?\\d+): closest beacon is at x=(-?\\d+), y=(-?\\d+)"); 
        std::smatch match; 
        while (std::getline(input_file, line)) {
            std::string::const_iterator search_it(line.cbegin());
            // This would be used to loop through the groups to search, e.g. if the pattern was 
            // "x=(-?\\d+), y=(-?\\d+)"
            // while (std::regex_search(search_stream, line.cend(), match, pattern)) {
            //     if (match.size() == 3) {
            //         long int sx = std::stoi(match[1].str()); 
            //         long int sy = std::stoi(match[2].str()); 
            //         // long int bx = std::stoi(match[3].str());
            //         // long int by = std::stoi(match[4].str());
            //         search_stream = match.suffix().first;
            //         // Sensor sensor(sx, sy, bx, by); 
            //         // sensors.push_back(sensor); 
            //     }
            // }
            if (std::regex_search(search_it, line.cend(), match, pattern) && match.size() == 5) {
                    long int sx = std::stoi(match[1].str()); 
                    long int sy = std::stoi(match[2].str()); 
                    long int bx = std::stoi(match[3].str());
                    long int by = std::stoi(match[4].str()); 
                    Sensor sensor(sx, sy, bx, by); 
                    sensors.push_back(sensor); 
            } 
        }
    } else {
        std::cerr << "Could not open the file." << std::endl; 
        exit(0); 
    }
    input_file.close(); 
} 

// Algorithms 
// Given a sensor, a row number, and the left/right extremes of the grid, it checks if the sensor's reception field 
// It exploits the symmetry of the Manatthan distance to calculate the specific coordinates of the interval at that 
// specific row 
auto find_sensor_reception_field(const Sensor& s, std::vector<Interval>& row_empty_coordinates, long int row_nr, long int max_left, long int max_right) {

    auto radius = s.distance; 
    if ((s.coordinate.second - radius) > row_nr || (s.coordinate.second + radius) < row_nr) return;  // the row is not in the sensor's reception field

    // Calculate how much the reception is extended horizontally
    auto distance_from_row = std::abs(s.coordinate.second - row_nr); 
    auto horizontal_extension = radius - distance_from_row;  
    // If the horizontal extremes are larger than the input ones, then the extremes are updated 
    auto left_extreme = s.coordinate.first - horizontal_extension; 
    auto right_extreme = s.coordinate.first + horizontal_extension; 
    left_extreme = (left_extreme < max_left) ? max_left : left_extreme; 
    right_extreme = (right_extreme > max_right) ? max_right : right_extreme;  
    // Push the interval in the coordinates vector
    Interval new_interval(left_extreme, right_extreme); 
    row_empty_coordinates.push_back(new_interval); 
}

auto detect_empty_positions_for_row(const std::vector<Sensor>& sensors, long int row_nr) {

    std::vector<Interval> row_empty_coordinates; 
    auto max_left = std::numeric_limits<long int>::min();   // for part 1 we don't need limits 
    auto max_right = std::numeric_limits<long int>::max(); 

    // Check the input row's specific empty positions by checking the reception field 
    // of each sensor at that row 
    for (const auto& s: sensors) {
        find_sensor_reception_field(s, row_empty_coordinates, row_nr, max_left, max_right); 
    }

    // Sort (ascending order) the found intervals 
    std::sort(row_empty_coordinates.begin(), row_empty_coordinates.end()); 

    // Now, it can be the case that there are overlapping intervals: therefore we need to adjust them 
    // and extend them if necessary. During the process we can count how many empty position 
    // each extended interval implies 
    long int number_empty_positions = 0; 
    Interval int_ = row_empty_coordinates.at(0);
    std::vector<Interval> extended_intervals; 
    size_t ind = 1; 

    while (ind < row_empty_coordinates.size()) {
        auto new_int_ = row_empty_coordinates.at(ind); 
        if (int_.extend(new_int_)) {    // <- if the interval is extended, continue extending 
            ++ind; 
            continue; 
        }
        // Else if there is no overlap, add the extended interval to the vector, 
        // update the number of empty positions by adding the interval size 
        // and start with the next interval 
        extended_intervals.push_back(int_); 
        number_empty_positions += int_.size(); 
        int_ = new_int_; 
        ++ind; 
    } 

    extended_intervals.push_back(int_);
    number_empty_positions += int_.size(); 

    // FOR DEBUGGING: 
    // std::cout << "Sorted intervals: " << std::endl; 
    // for (const auto& e: row_empty_coordinates) {
    //     e.print(); 
    //     std::cout << e.size() << std::endl; 
    // }

    // std::cout << "Extended intervals: " << std::endl; 
    // for (const auto& e: extended_intervals) {
    //     e.print(); 
    //     std::cout << e.size() << std::endl;
    // }


    // for (long int i = 9; i <= 11; ++i) {
    //     for (const auto& e: empty_coordinates[i]) {
    //         std::cout << std::to_string(e) << " "; 
    //     }
    //     std::cout << std::endl;
    // }

    // Last, we need to check if a sensor or a beacon are in the interval's coordinates, 
    // in which case we have to rule them out. As some coordinate and/or beacon can be repeated, 
    // we need to store them in a vector in order to be sure not to double count them 
    auto coordinate_is_in_vector = [](std::vector<Coordinate> v, const Coordinate& c) {
        return std::find_if(v.begin(), v.end(), [&c](const Coordinate& p) {
            return p.first == c.first && p.second == c.second; 
        }) != v.end(); 
    };

    std::vector<Coordinate> sensors_and_beacons_in_row; 
    for (const auto& int_it: extended_intervals) {
        for (const auto& s: sensors) {

            auto& c = s.coordinate; 
            auto& b = s.closest_beacon; 
        
            if (c.second == row_nr && !coordinate_is_in_vector(sensors_and_beacons_in_row, c)) {
                if (int_it.contains(c.first)) {
                    --number_empty_positions;   // if sensor in interval: decrease the empty positions count
                    sensors_and_beacons_in_row.push_back(c); 
                }
            }

            if (b.second == row_nr && !coordinate_is_in_vector(sensors_and_beacons_in_row, b)) {
                if (int_it.contains(b.first)) {
                    --number_empty_positions; // if beacon in interval: decrease the empty positions count
                    sensors_and_beacons_in_row.push_back(b); 
                }
            }
        }
    }

    return number_empty_positions; 
}

void solve_part_one(const std::vector<Sensor>& sensors, long int row_nr) {
    std::cout << "start computing..." << std::endl; 
    auto solution = detect_empty_positions_for_row(sensors, row_nr); 
    std::cout << "The solution to part one is " << solution << std::endl; 
}

auto find_distress_signal(std::vector<Sensor>& sensors, const std::vector<long int>& extremes) -> long int{

    auto max_left = extremes.at(0); 
    auto max_right = extremes.at(1); 
    auto max_up = extremes.at(2); 
    auto max_down = extremes.at(3); 

    // Loop across each of the possible rows (max_up to max_down)
    for (size_t row_nr = max_up; row_nr <= max_down; ++row_nr) {
        std::vector<Interval> row_empty_coordinates; 

        // For the current row, check all the interval of the empty positions 
        for (const auto& s: sensors) {
            find_sensor_reception_field(s, row_empty_coordinates, row_nr, max_left, max_right); 
        }

        // Sort the intervals in the ascending orders and, as done for part 1, 
        // avoid overlapping intervals by extending the first one 
        std::sort(row_empty_coordinates.begin(), row_empty_coordinates.end()); 
        long int number_empty_positions = 0; 
        Interval int_ = row_empty_coordinates.at(0);
        std::vector<Interval> extended_intervals; 
        size_t ind = 1; 
        while (ind < row_empty_coordinates.size()) {
            auto new_int_ = row_empty_coordinates.at(ind); 

            if (int_.extend(new_int_)) {
                ++ind; 
                continue; 
            }

            extended_intervals.push_back(int_); 
            number_empty_positions += int_.size(); 
            int_ = new_int_; 
            ++ind; 
        } 

        extended_intervals.push_back(int_);
        number_empty_positions += int_.size(); 

        // Now check that the final extended interval does not leave non-empty positions: 
        // if there is one empty position we have found the row containing the distressed signal 
        // The column will simply be the superior extreme of the first interval + 1 (a non-empty position)
        // After that we can calculate the corresponding tuning frequency
        if (number_empty_positions != std::abs(max_right-max_left)+1) { 
            auto col = extended_intervals.at(0).sup + 1; 
            // std::cout << "X: " << col << " Y: " << row_nr << std::endl; 
            return col * 4000000 + row_nr; 

        }
    }
    return 0; 

}

void solve_part_two(std::vector<Sensor>& sensors, const std::vector<long int>& extremes) {
    auto solution = find_distress_signal(sensors, extremes);
    std::cout << "The solution to part two is " << solution << std::endl; 
}


