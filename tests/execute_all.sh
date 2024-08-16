#!/bin/bash

T_RESET=$'\e[0m'

T_BRED=$'\e[1;31m'
T_BBLUE=$"\e[1;34m"
T_BGREEN=$'\e[1;32m'
T_BYLLW=$"\e[1;33m"

INPUT_DIR="./inputs/"
INPUT_FILES="${INPUT_DIR}*.txt"

# Build executable at ../build
cd ..
mkdir -p build
cd build
cmake ..
make
cd ../tests
echo "${T_BGREEN}SUCCESSFULLY BUILT${T_RESET}"
KEPLER="../build/KeplerKV"

# Make a directory for storing the results
mkdir -p results
RESULTS_DIR="./results/"

printf "%-25s %s\n----------------------------------------\n" "TEST CASE" "RESULT"
for input_file in $INPUT_FILES
do
    no_path=$(basename "$input_file")
    name_base="${no_path%.*}"

    out_file="./outputs/${name_base}_out.txt"
    res_file="${RESULTS_DIR}${name_base}_result.txt"
    diff_file="${RESULTS_DIR}${name_base}_diff.txt"

    # echo -en "$name_base"

    # sed to remove ANSI escape sequences: https://superuser.com/a/380778
    $KEPLER < "$input_file" 2>&1| sed -e 's/\x1b\[[0-9;]*m//g' | sed 's/^> //' &> "$res_file"
    
    # Find any error messages
    grep -qE "Segmentation fault|Aborted|Assertion failed|\
    Illegal instruction|Floating point exception|Bus error|Memory error|\
    File not found|Permission denied|Command not found|Invalid argument|terminate called" "$res_file"
    if [ $? -eq 0 ]; then
        printf "%-25s %s\n" "$name_base" "${T_BRED}FAILED: program crash${T_RESET}"
        continue
    fi

    diff -wB $out_file "$res_file" > "$diff_file"
    if [ $? -eq 0 ]; then
        printf "%-25s %s\n" "$name_base" "${T_BGREEN}PASSED${T_RESET}"
        # echo -e "\t\t${T_BGREEN}PASSED${T_RESET}"
    else
        printf "%-25s %s\n" "$name_base" "${T_BRED}FAILED: wrong output${T_RESET}"
    fi
    
done
