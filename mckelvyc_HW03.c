/*
Name: Conner McKelvy
BlazerId: mckelvyc
Project: Homework 03
To compile: make
To run: ./hw3 <directory_name>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int isTextFile(char *fileName)
{
    size_t length = strlen(fileName);

    if (length >= 4 && strcmp(fileName + length - 4, ".txt") == 0) {
        return 1;
    }

    return 0;
}

char *createPath(char *directoryName, char *fileName)
{
    size_t directoryLength = strlen(directoryName);
    size_t fileLength = strlen(fileName);
    size_t pathLength = directoryLength + fileLength + 2;
    char *path = malloc(pathLength);
    size_t i;
    size_t position = 0;

    if (path == NULL) {
        return NULL;
    }

    for (i = 0; i < directoryLength; i++) {
        path[position] = directoryName[i];
        position++;
    }

    if (directoryName[directoryLength - 1] != '/') {
        path[position] = '/';
        position++;
    }

    for (i = 0; i < fileLength; i++) {
        path[position] = fileName[i];
        position++;
    }

    path[position] = '\0';

    return path;
}

long long countWords(char *path)
{
    FILE *file;
    int character;
    int insideWord = 0;
    long long wordCount = 0;

    file = fopen(path, "r");

    if (file == NULL) {
        return -1;
    }

    while ((character = getc(file)) != EOF) {
        if (
            character == ' ' ||
            character == '\n' ||
            character == '\t'
        ) {
            insideWord = 0;
        } else if (insideWord == 0) {
            wordCount++;
            insideWord = 1;
        }
    }

    fclose(file);

    return wordCount;
}

void printFileInformation(char *path, char *fileName)
{
    struct stat fileInformation;
    long long wordCount;

    if (stat(path, &fileInformation) == -1) {
        printf("Error: Cannot access file %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    if (isTextFile(fileName)) {
        wordCount = countWords(path);

        if (wordCount == -1) {
            printf(
                "Error: Cannot open or read file %s\n",
                fileName
            );

            exit(EXIT_FAILURE);
        }

        printf(
            "File: %s | Size: %lld bytes | Words: %lld\n",
            fileName,
            (long long)fileInformation.st_size,
            wordCount
        );
    } else {
        printf(
            "File: %s | Size: %lld bytes\n",
            fileName,
            (long long)fileInformation.st_size
        );
    }

    exit(0);
}

int main(int argc, char **argv)
{
    DIR *directory;
    struct dirent *entry;
    struct stat fileInformation;
    pid_t pid;
    pid_t waitedPid;
    char *path;
    int childCount = 0;
    int childError = 0;
    int status;
    int i;

    if (argc != 2) {
        printf("Usage: ./hw3 <directory_name>\n");
        exit(EXIT_FAILURE);
    }

    directory = opendir(argv[1]);

    if (directory == NULL) {
        printf("Error: Directory not found.\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(directory)) != NULL) {
        if (
            strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0
        ) {
            continue;
        }

        path = createPath(argv[1], entry->d_name);

        if (path == NULL) {
            printf("Error: Memory could not be allocated.\n");

            closedir(directory);

            for (i = 0; i < childCount; i++) {
                wait(&status);
            }

            exit(EXIT_FAILURE);
        }

        if (stat(path, &fileInformation) == -1) {
            printf(
                "Error: Cannot access file %s\n",
                entry->d_name
            );

            free(path);
            continue;
        }

        if ((fileInformation.st_mode & S_IFMT) == S_IFREG) {
            pid = fork();

            if (pid == 0) {
                printFileInformation(
                    path,
                    entry->d_name
                );
            } else if (pid > 0) {
                childCount++;
            } else {
                perror("fork");

                free(path);
                closedir(directory);

                for (i = 0; i < childCount; i++) {
                    wait(&status);
                }

                exit(EXIT_FAILURE);
            }
        }

        free(path);
    }

    closedir(directory);

    for (i = 0; i < childCount; i++) {
        waitedPid = wait(&status);

        if (waitedPid == -1) {
            perror("wait");
            childError = 1;
            break;
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                childError = 1;
            }
        } else {
            childError = 1;
        }
    }

    if (childError == 1) {
        return EXIT_FAILURE;
    }

    return 0;
}