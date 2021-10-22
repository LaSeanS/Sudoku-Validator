My program reads a text file named "solution.txt" in order to execute properly. The "solution.txt" file must be in the same folder as the sudoku_validator.c file, and must follow the same format as the text file provided in the assignment description. (a 9x9 matrix of integers).

The command to compile and execute the program (on a Linux system) is:

gcc sudoku_validator.c -lpthread -o output.out

From here, you can print the output to the command line using:

./output.out
