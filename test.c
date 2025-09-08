#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char n[] = "hello apple orange banana";  // Make it mutable
    char* token;
    char** arr = malloc(sizeof(char*) * 10);  // Allocate space for 10 tokens
    int i = 0;

    token = strtok(n, " ");
    while (token != NULL) {
        arr[i] = token;  // Store token in array
        i++;
        token = strtok(NULL, " ");  // Get next token
    }

    // Print the tokens
    for (int j = 0; j < i; j++) {
        printf("%s\n", arr[j]);
    }

    // Free the allocated memory
    free(arr);
    return 0;
}

