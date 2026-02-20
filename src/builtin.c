/******************************************************************************
 * File Name: builtin.c
 * 
 * Description: Built in functions for the bsh shell.
 * 
 * Author: Dennis Shelby 
 * 
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <readline/history.h>
#include "bsh.h"
#include "builtin.h"



/******************************************************************************
 * Function Name: bsh_history()
 * 
 * Description: Wrapper for print_commands().
 * 
 *****************************************************************************/
void bsh_history() {
    for (int i = 0; i < history_length; i++) {
        HIST_ENTRY *entry = history_get(i);
        if (entry != NULL) {
            printf("%s\n", entry->line);
        }
    }
}


/******************************************************************************
 * Function Name: bsh_status()
 * 
 * Description: Prints the exit value of the last command to complete.
 * 
 *****************************************************************************/
void bsh_status(int *childStatus) {
    printf("exit value %d\n", WEXITSTATUS(*childStatus));
    fflush(stdout);
}

/******************************************************************************
 * Function Name: bsh_cd()
 * 
 * Description: Change working directory.
 * 
 *****************************************************************************/
void bsh_cd(Command *curCmd) {
    if (curCmd->argc > 1) {
        // if (!strcmp(curCmd->argv[1][0], "~")) {
        if (curCmd->argv[1][0] == '~') {
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
