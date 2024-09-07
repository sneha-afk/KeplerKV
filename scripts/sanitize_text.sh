#!/bin/bash

# Removes ANSI color sequences and '> ' that is present in KeplerKV's output
sanitize_text() {
    sed -e 's/\x1b\[[0-9;]*m//g' | sed 's/^> //'
}

if [ $# -eq 1 ]; then
    # Input file provided, read from file
    input_file="$1"
    if [ -f "$input_file" ]; then
        cat "$input_file" | sanitize_text
    else
        echo "File not found: $input_file"
        exit 1
    fi
else
    # Read from stdin
    sanitize_text
fi
