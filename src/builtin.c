#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "bsh.h"

int builtin(Command *curCmd, int *childStatus) {

    if (!strcmp(curCmd->argv[0], "exit")) {
            return 2;
        } else if (!strcmp(curCmd->argv[0], "cd")) {
            if (curCmd->argc > 1) {
                if (!strcmp(curCmd->argv[1], "~")) {
                    chdir(getenv("HOME"));
                }
                else if (chdir(curCmd->argv[1]) == -1) {
                    printf("%s: No such file or directory\n", curCmd->argv[1]);
                    fflush(stdout);
                }
            } else {
                chdir(getenv("HOME"));
            }
            return 1;
        } else if (!strcmp(curCmd->argv[0], "status")) {
            printf("exit value %d\n", WEXITSTATUS(*childStatus));
            fflush(stdout);
            return 1;
        }
    
    return 0;
}