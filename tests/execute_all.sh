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

for input_file in $INPUT_FILES
do
    no_path=$(basename "$input_file")
    name_base="${no_path%.*}"

    out_file="./outputs/${name_base}_out.txt"
    res_file="${RESULTS_DIR}${name_base}_result.txt"
    diff_file="${RESULTS_DIR}${name_base}_diff.txt"

    echo -e "NOW TESTING:\t$name_base"

    # sed to remove ANSI escape sequences: https://superuser.com/a/380778
    $KEPLER < "$input_file" | sed -e 's/\x1b\[[0-9;]*m//g' > "$res_file"
    diff $out_file "$res_file" > "$diff_file"

    if [ $? -eq 0 ]; then
        echo -e "\t${T_BGREEN}PASSED${T_RESET}"
    else
        echo -e "\t${T_BRED}FAILED${T_RESET}"
    fi
done
