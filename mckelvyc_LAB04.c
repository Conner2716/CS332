#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFSIZE 4096

int main(int argc, char *argv[]) {
    int file1Descriptor, file2Descriptor;
    long int n;
    char buf[BUFFSIZE];

    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        exit(-1);
    }

    if (strcmp(argv[1], argv[2]) == 0) {
        printf("Error: The two filenames cannot be the same.\n");
        exit(-1);
    }

    file1Descriptor = open(argv[1], O_WRONLY | O_APPEND);
    file2Descriptor = open(argv[2], O_RDONLY);

    if (file1Descriptor == -1 || file2Descriptor == -1) {
        printf("Error with file open\n");
        exit(-1);
    }

    while ((n = read(file2Descriptor, buf, BUFFSIZE)) > 0) {
        if (write(file1Descriptor, buf, n) != n) {
            printf("Error writing to file1\n");
            exit(-1);
        }
    }

    if (n < 0) {
        printf("Error reading file2\n");
        exit(-1);
    }

    close(file1Descriptor);
    close(file2Descriptor);

    return 0;
}