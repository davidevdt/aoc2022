#!/bin/bash 

is_test=0

run_single_day() {
    local dir=$1
    if [ "$#" -ge 2 ]; then 
        if [ "$2" == "test" ]; then
            find "./" -maxdepth 1 -type f -name "$dir" -exec {} test \; 
        else 
            echo "Invalid argument."
        fi 
    else 
        find "./" -maxdepth 1 -type f -name "$dir" -exec {} \;
    fi
}

cd ./build/
for dir in day*; do 
    cd "./$dir"
    if [ "$#" -ge 1 ]; then
        if [ "$1" == "test" ]; then
            cp "../../$dir/test.txt" "./"
            is_test=1
        else 
            cp "../../$dir/input.txt" "./"
        fi
    else 
        cp "../../$dir/input.txt" "./"
    fi

    if [[ $dir =~ day*([0-9]+) ]]; then 
        if [ "$is_test" -eq 1 ]; then
            run_single_day "$dir" "test" 
        else 
            run_single_day "$dir"
        fi
    fi
    cd .. 
done

