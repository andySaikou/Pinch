// Main function for the pinch interpreter

#include <stdio.h>

int main(int argc, char *argv[]) {
    
    // Argument checking
    // Interpreter expects zero or one additional argument
    // ./pinch launches the interpreter in interactive mode
    // ./pinch <filename> interpretes a specified file

    if (argc > 2) {
        printf("Too many arguments provided.\n");
        return 1;
    }
    else if (argc == 2) {
        // Check if <filename> is a valid file
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            printf("File not found: %s\n", argv[1]);
            return 1;
        }

        // Interpret the specified file
        // TODO
        printf("Interpreting file: %s\n", argv[1]);
    }
    else if (argc == 1) {
        // Launch interactive mode
        // TODO
        printf("Launching interactive mode...\n");
    }
}