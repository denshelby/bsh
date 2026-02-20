/******************************************************************************
 * File Name: builtin.c
 * 
 * Description: Built in functions for the bsh shell and helper functions for 
 * the builtins.
 * 
 * Author: Dennis Shelby 
 * 
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "bsh.h"
#include "builtin.h"


/******************************************************************************
 * Function Name: add_command()
 * 
 * Description: Adds a command to the circular array of commands. Ignores
 * consecutive duplicate commands.
 * 
 *****************************************************************************/
void add_command(CommandArray *cmds, char *command) {
    
    if (strcmp(cmds->arr[(cmds->curr - 1) % MAXCOMMANDS], command)) {   // Ignore dup
        strcpy(cmds->arr[cmds->curr], command);

        if (++cmds->count >= MAXCOMMANDS) {
            cmds->start = (cmds->curr + 1) % MAXCOMMANDS;
            cmds->count = MAXCOMMANDS;
        }

        cmds->curr = (cmds->curr + 1) % MAXCOMMANDS;
    }
}

/******************************************************************************
 * Function Name: print_commands()
 * 
 * Description: prints the commands stored in the circular array of commands
 * starting at cmds->start. Does not print the last command added to the array.
 * This command will always be "history" because that is the bsh command that 
 * triggers this function.
 * 
 *****************************************************************************/
void print_commands(CommandArray *cmds) {
    for (int i = 0; i < cmds->count-1; i++) {
        printf("%s", cmds->arr[(cmds->start + i) % MAXCOMMANDS]);
    }
}


/******************************************************************************
 * Function Name: bsh_history()
 * 
 * Description: Wrapper for print_commands().
 * 
 *****************************************************************************/
void bsh_history(CommandArray *cmds) {
    print_commands(cmds);
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
