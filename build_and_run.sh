#!/bin/bash

day="day$1"
cd "./$day"
g++ "$day.cpp" -o "$day" 
if [ "$2" == "test" ]; then
    "./$day" "test" 
else 
    "./$day"
fi
rm "$day"