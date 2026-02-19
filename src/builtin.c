#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "bsh.h"
#include "builtin.h"

void add_command(CommandArray *cmds, char *command) {
    
    if (strcmp(cmds->arr[(cmds->curr - 1) % MAXCOMMANDS], command)) {
        strcpy(cmds->arr[cmds->curr], command);

        if (++cmds->count >= MAXCOMMANDS) {
            cmds->start = (cmds->curr + 1) % MAXCOMMANDS;
            cmds->count = MAXCOMMANDS;
        }

        cmds->curr = (cmds->curr + 1) % MAXCOMMANDS;
    }
}

void print_commands(CommandArray *cmds) {
    for (int i = 0; i < cmds->count-1; i++) {
        printf("%s", cmds->arr[(cmds->start + i) % MAXCOMMANDS]);
    }
}

void bsh_history(CommandArray *cmds) {
    print_commands(cmds);
}


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
