#!/bin/bash

# Check if the main C file is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 [-d] [-p] [-e <macro1=value> -e <macro2=value> ...] <main.c>"
    exit 1
fi

# Initialize options
debug=false
use_pthread=false
macros=""

# Parse options
while [[ "$1" == -* ]]; do
    case "$1" in
        -d) debug=true ;;  # Debug flag
        -p) use_pthread=true ;;  # Enable pthread linking
        -e) shift; macros="$macros -D$1" ;;  # Accumulate macros
        *) echo "Unknown option: $1" ; exit 1 ;;
    esac
    shift
done

main_file="$1"
base_name=$(basename "$main_file" .c)  # Extract the base name without extension

# Create directories for object files and executable if they don't exist
obj_dir="obj"
bin_dir="bin"
mkdir -p "$obj_dir"
mkdir -p "$bin_dir"

# Compile the main C file with accumulated macros
gcc $macros -c "$main_file" -o "$obj_dir/${base_name}.o"
if [ $? -ne 0 ]; then
    echo "Error compiling $main_file"
    exit 1
fi

# Link the object files and create the executable in the bin directory
link_flags=""
if $use_pthread; then
    link_flags="-pthread"
fi

gcc -o "$bin_dir/${base_name}" "$obj_dir/${base_name}.o" -lc $link_flags
if [ $? -ne 0 ]; then
    echo "Error linking object files"
    exit 1
fi

echo "Build successful! Executable created: $bin_dir/${base_name}"

# Run the program, with or without gdb
if $debug; then
    gdb "$bin_dir/${base_name}"
else
    "$bin_dir/${base_name}"
fi

