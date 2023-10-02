# Advent Of Code 2022 

This repo contains my solutions to [Advent Of Code 2022](https://adventofcode.com/). I chose the  `C++` language (minimum version C++11) to solve the puzzles. 

About the approach I used: 

* I prioritized correctness over efficiency 
* Each day consists of two puzzles. Since these usually require the same input data, I opted to import the data once and process it twice instead of processing it during the import stage to avoid redundant imports.
* It is also possible to run test cases (see below for instructions).

## How to compile and run the code 
On Linux, you can run the code for all days by executing the following commands after downloading the repository:

```$ ./build.sh``` <br>
```$ ./aoc2022.sh``` 

To run the test cases, you can use:

```$ ./aoc2022.sh test```

This will also display the initial data configuration to ensure the data has been imported correctly. To clean the build, run

`$ ./clean.sh` 

Alternatively, you can compile (e.g., with gcc) and run each single day as follows: 

```$ g++ ./day1/day1.cpp -o ./day1/day1```<br>
```$ ./day1/day1```

Replace "1" with the desired day number. To run the test case, you can use: 

```$ ./day1/day1 test```
