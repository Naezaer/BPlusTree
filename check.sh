#!/bin/bash

build_dir="./build"
testdata_dir="./testdata"
result_dir="$build_dir/output"
input_dir="$testdata_dir/input"
output_dir="$testdata_dir/output"

if [ ! -d "$build_dir" ]; then
    echo "Project has not been built yet."
    return 1
fi

if [ ! -d "$result_dir" ]; then
    mkdir -p "$result_dir"
fi

if [ $# -eq 0 ]; then
    input_files=("$input_dir"/*.txt)
else
    input_files=()
    for arg in "$@"; do
        if [[ $arg =~ ^[0-9]+$ ]]; then
            input_files+=("$input_dir/$arg.txt")
        elif [[ $arg =~ ^([0-9]+)-([0-9]+)$ ]]; then
            start=${BASH_REMATCH[1]}
            end=${BASH_REMATCH[2]}
            for ((i=start; i<=end; i++)); do
                input_files+=("$input_dir/$i.txt")
            done
        fi
    done
fi

failed=false

for input_file in "${input_files[@]}"; do
    filename=$(basename "$input_file" .txt)
    result_file="$result_dir/$filename.txt"
    output_file="$output_dir/$filename.txt"

    line_count=$(grep -cve '^\s*$' "$input_file")

    # ./build/BPlusTree < "$input_file" > "$result_file"
    {
        echo "$line_count"
        cat "$input_file"
    } | ./build/BPlusTree > "$result_file"

   #  echo >> "$result_file"

    diff "$result_file" "$output_file"
    if [ $? -eq 0 ]; then
        echo "Test $filename: Success!"
    else
        echo "Test $filename: Fail!"
        failed=true
    fi
done

# rm -rf "$result_dir"

if $failed; then
    exit 1
else
    exit 0
fi

