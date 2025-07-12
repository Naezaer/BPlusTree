# Implementation of Basic B+ Tree Functions (C Version)

## Features
- **Insert Operation**: Supports inserting key-value pairs into the B+ tree. Nodes will be automatically split when they reach maximum capacity.
- **Delete Operation**: Supports deleting specified keys from the B+ tree. Nodes will be merged when the number of keys in a node is too small.
- **Equality Query**: Searches for the corresponding value based on a given key. Returns -1 if the key does not exist.
- **Range Query**: Searches for key-value pairs within a given key range, stores the results in a buffer, and returns the total number of results.
- **Tree Height Query**: Returns the height of the B+ tree.
- **Debugging Function**: Provides a function to print the tree structure, facilitating debugging and viewing the tree's status.

## Code Structure
```plaintext
BPlusTree/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   ├── BPlusTree.h
│   └── BPlusTree.c
```
- `CMakeLists.txt`: CMake configuration file for compiling the project.
- `src/main.c`: Main program file, which can be used to test the functions of the B+ tree.
- `src/BPlusTree.h`: Header file of the B+ tree, defining node structures, tree structures, and declarations of various operation functions.
- `src/BPlusTree.c`: Implementation file of the B+ tree, containing specific implementations of various operation functions.

## Compilation
```bash
mkdir build
cd build
cmake ..
make
```

## Testing and Usage
Use the `check.sh` script to verify if the program output is correct using IO redirection and the `diff` command:
```bash
chmod +x check.sh
# Single test case
./check.sh 1
# Range of test cases
./check.sh 3-6
# All test cases
./check.sh
```
