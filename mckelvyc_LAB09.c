#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

pid_t pid;

static void sig_handler(int signo) {
    signal(signo, SIG_IGN);

    switch(signo) {
        case SIGINT:
            kill(pid, SIGINT);
            printf("Child process interrupted\n");
            break;

        case SIGTSTP:
            kill(pid, SIGTSTP);
            printf("Child process suspended\n");
            break;

        case SIGQUIT:
            printf("Parent process received quit signal\n");
            fflush(stdout);
            exit(0);

        default:
            printf("received signal %d\n", signo);
    }

    fflush(stdout);
    signal(signo, sig_handler);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }

    pid = fork();

    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("exec");
        exit(-1);
    } else if (pid > 0) {
        if (signal(SIGINT, sig_handler) == SIG_ERR) {
            printf("Unable to catch SIGINT\n");
            exit(-1);
        }

        if (signal(SIGTSTP, sig_handler) == SIG_ERR) {
            printf("Unable to catch SIGTSTP\n");
            exit(-1);
        }

        if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
            printf("Unable to catch SIGQUIT\n");
            exit(-1);
        }

        printf("Wait for a quit signal\n");

        for ( ; ; )
            pause();
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}