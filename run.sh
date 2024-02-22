#!/bin/bash

# Get the directory path of the current script file
# SCRIPT_DIR=$(dirname "$(readlink -f "$0")")

# Specify the list of C files you want to compile and run
c_files=("server.c" "auth.c" "dns.c" "user.c" "user.c" "dns.c" "user.c" "user.c")
# c_files=("server.c" "auth.c" "dns.c" "user.c")

# Specify the corresponding list of arguments for each C file
# Use empty string ("") if no arguments are needed for a particular file
loop_back="127.0.0.1"
target="$loop_back 12300"
auth="$loop_back 20000"
local11="$loop_back 30000"
local12="$loop_back 40000"
local21="$loop_back 50000"
local22="$loop_back 60000"
c_args=("$target $auth" "$auth $target" "$local11 $local12 $auth $target" "$target $local11" "$target $local11" "$local21 $local22 $auth $target" "$target $local21" "$target $local21")
# c_args=("$target $auth" "$auth $target" "$local11 $local12 $auth $target" "$target $local11")

# Check if the lengths of the two lists are equal
if [ "${#c_files[@]}" -ne "${#c_args[@]}" ]; then
    echo "Error: Number of source files and argument lists do not match."
    exit 1
fi

# Create an array to hold the PIDs of the background processes
pids=()

# Loop through the list of C files
for i in "${!c_files[@]}"; do
  file="${c_files[$i]}"
  args="${c_args[$i]}"

  # Compile the C file using gcc
  gcc -o "${file%.c}" "$file" -lssl -lcrypto

  # Check if the compilation was successful
  if [ $? -eq 0 ]; then
    # Set execute permissions for the compiled program
    sudo chmod +x "./${file%.c}"

    # Run the compiled program with arguments in the background
    gnome-terminal --title="${file%.c}" --geometry=40x12 -- bash -c "./${file%.c} $args"

    # Store the PID of the background process
    pids+=($!)
  else
    echo "Compilation of $file failed."
  fi
done

# Wait for all background processes to finish
for pid in "${pids[@]}"; do
  wait "$pid"
done

echo "All processes have completed. "
