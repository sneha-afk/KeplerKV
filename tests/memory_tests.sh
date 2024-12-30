#!/bin/bash

T_RESET=$'\e[0m'
T_BRED=$'\e[1;31m'
T_BBLUE=$'\e[1;34m'
T_BGREEN=$'\e[1;32m'
T_BYLLW=$'\e[1;33m'

echo "${T_BBLUE}Check for memory leaks with valgrind.${T_RESET}"

# Build executable at ../build
cd ..
mkdir -p build
cd build
cmake ..
make clean
make

if [ $? -eq 0 ]; then
    cd ../tests
    echo "${T_BGREEN}SUCCESSFULLY BUILT${T_RESET}"
else
    echo "${T_BRED}ERROR BUILDING${T_RESET}"
    exit 1
fi

KEPLER="../build/KeplerKV"

INPUT_DIR="./inputs/"
INPUT_FILES="${INPUT_DIR}*.kep"
CLEAN_OUT="../scripts/sanitize_text.sh"

# Make a directory for storing the results
mkdir -p results
RESULTS_DIR="./results/"

printf "%-25s %s\n----------------------------------------\n" "TEST CASE" "RESULT"
for input_file in $INPUT_FILES
do
    no_path=$(basename "$input_file")
    name_base="${no_path%.*}"

    res_file="${RESULTS_DIR}${name_base}_memory_result.txt"

    valgrind -s --leak-check=full $KEPLER "$input_file" 2>&1| ${CLEAN_OUT} &> "$res_file"

    if [ $? -eq 0 ]; then
        printf "%-25s %s\n" "$no_path" "${T_BGREEN}PASSED${T_RESET}"
    else
        printf "%-25s %s\n" "$no_path" "${T_BRED}FAILED: wrong output${T_RESET}"
    fi
    
done
