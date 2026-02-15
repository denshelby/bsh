#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "bsh.h"


void bsh_status(int *childStatus) {
    printf("exit value %d\n", WEXITSTATUS(*childStatus));
    fflush(stdout);
}

void bsh_cd(Command *curCmd) {
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

}

void bsh_history(){

}