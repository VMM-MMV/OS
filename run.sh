#!/bin/bash

# Check if the main assembly file is provided
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 [-d] [-l] <main.asm>"
    exit 1
fi

# Initialize options
debug=false
link_io=false

# Parse options
while [[ "$1" == -* ]]; do
    case "$1" in
        -d) debug=true ;;
        -l) link_io=true ;;
        *) echo "Unknown option: $1" ; exit 1 ;;
    esac
    shift
done

main_file="$1"
base_name=$(basename "$main_file" .asm)  # Extract the base name without extension

# Create directories for object files and executable if they don't exist
obj_dir="obj"
bin_dir="bin"
mkdir -p "$obj_dir"
mkdir -p "$bin_dir"

# Assemble io.asm if -l is set
if $link_io; then
    nasm -f elf64 io.asm -o "$obj_dir/io.o"
    if [ $? -ne 0 ]; then
        echo "Error assembling io.asm"
        exit 1
    fi
fi

# Assemble the main assembly file
nasm -f elf64 "$main_file" -o "$obj_dir/${base_name}.o"
if [ $? -ne 0 ]; then
    echo "Error assembling $main_file"
    exit 1
fi

# Link the object files and create the executable in the bin directory
if $link_io; then
    ld -o "$bin_dir/${base_name}" "$obj_dir/${base_name}.o" "$obj_dir/io.o" -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2
else
    ld -o "$bin_dir/${base_name}" "$obj_dir/${base_name}.o" -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2
fi

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

