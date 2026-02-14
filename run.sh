#!/bin/bash

# Enable recursive globbing in bash (optional, can also use 'find')
shopt -s globstar

# Loop through all subdirectories
for dir in **/; do
    # Check if a Makefile exists in the directory
    if [[ -f "$dir/Makefile" ]]; then
        echo "Entering directory: $dir"
        # Run make in that specific directory using the -C option
        # make -C "$dir"
        # Or, alternatively, change directory and run make
        (cd "$dir" && make "$1")
    fi
done
