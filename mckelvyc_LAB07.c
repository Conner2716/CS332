#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_ARGS 100

int main(int argc, char **argv)
{
    FILE *inputFile;
    FILE *logFile;

    char line[BUFSIZ];
    char command[BUFSIZ];
    char *args[MAX_ARGS];
    char *token;
    char *timeString;

    int argCount;
    int status;
    int i;

    pid_t pid;

    time_t startTime;
    time_t endTime;

    if (argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inputFile = fopen(argv[1], "r");

    if (inputFile == NULL) {
        perror("fopen input file");
        exit(EXIT_FAILURE);
    }

    logFile = fopen("output.log", "w");

    if (logFile == NULL) {
        perror("fopen output.log");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, BUFSIZ, inputFile) != NULL) {

   
        i = 0;

        while (line[i] != '\0' && line[i] != '\n') {
            command[i] = line[i];
            i++;
        }

        command[i] = '\0';

        argCount = 0;
        token = strtok(line, " \t\n");

        while (token != NULL && argCount < MAX_ARGS - 1) {
            args[argCount] = token;
            argCount++;

            token = strtok(NULL, " \t\n");
        }

        args[argCount] = NULL;

        if (argCount == 0) {
            continue;
        }

        startTime = time(NULL);

        pid = fork();

        if (pid == 0) {
            execvp(args[0], args);

            perror("execvp");
            exit(EXIT_FAILURE);

        } else if (pid > 0) {
            wait(&status);

            endTime = time(NULL);

            if (WIFEXITED(status)) {
          
                timeString = ctime(&startTime);
                timeString[strlen(timeString) - 1] = '\0';

                fprintf(logFile, "%s\t%s\t", command, timeString);

                timeString = ctime(&endTime);
                timeString[strlen(timeString) - 1] = '\0';

                fprintf(logFile, "%s\n", timeString);

                printf("%s exited with status = %d\n",
                       args[0], WEXITSTATUS(status));

            } else {
                printf("Child process did not terminate normally!\n");
            }

        } else {
            perror("fork");

            fclose(inputFile);
            fclose(logFile);

            exit(EXIT_FAILURE);
        }
    }

    fclose(inputFile);
    fclose(logFile);

    return 0;
}