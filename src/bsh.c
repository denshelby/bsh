/******************************************************************************
 * Program Name: bsh
 * 
 * Description: Simple Linux shell. Provides basic shell functionality, 
 * including executing built-in commands like `cd`, `status`, and `exit`, 
 * executing external commands, handling input/output redirection, and 
 * managing background processes.
 * 
 * Author: Dennis Shelby
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>

#include "bsh.h"
#include "builtin.h"

int main() {
    int childStatus = 0;
    int inFD = dup(0);
    int outFD = dup(1);
    ActivePID *activehead = NULL;

    // Ignore SIGINT
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &ignore_action, NULL);

    // SIGINT kill bg processes
    struct sigaction fgsigint = {0};
    fgsigint.sa_handler = handle_fgsigint;
    fgsigint.sa_flags = 0;

    // SIGTSTP
    struct sigaction bgsigtstp = {0};
    bgsigtstp.sa_handler = handle_bgsigtstp;
    bgsigtstp.sa_flags = 0;
    sigaction(SIGTSTP, &bgsigtstp, NULL);


    while (true) {
        processCheck(activehead);

        Command *curCmd = getInput();

        // Ignore blank line or comment
        if (curCmd->argc == 0) continue;
        if (!strncmp(curCmd->argv[0], "#", 1)) continue;


        if (!strcmp(curCmd->argv[0], "exit")) {
            free(curCmd);
            break;
        } else if (!strcmp(curCmd->argv[0], "status")) {
            bsh_status(&childStatus);
        } else if (!strcmp(curCmd->argv[0], "cd")) {
            bsh_cd(curCmd);
        } else if (!strcmp(curCmd->argv[0], "history")) {
            bsh_history();
        
        // Process exec() commands
        } else {
            int sourceFD;
            int targetFD;

            // Open input file if redirect is in command
            if (strlen(curCmd->infile) > 0) {
                sourceFD = open(curCmd->infile, O_RDONLY);
                if (sourceFD == -1) {
                    perror("source open()");
                    fflush(stdout);
                    continue;
                }
                int result = dup2(sourceFD, STDIN_FILENO);
                if (result == -1) {
                    perror("source dup2()");
                    fflush(stdout);
                    continue;
                }
            }

            // Open output file if redirect is in command
            if (strlen(curCmd->outfile) > 0) {
                targetFD = open(curCmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (targetFD == -1) {
                    perror("source open()");
                    fflush(stdout);
                    continue;
                }
                int result = dup2(targetFD, STDOUT_FILENO);
                if (result == -1) {
                    perror("target dup2()");
                    fflush(stdout);
                    continue;
                }
            }

            // Create child process and run exec() command
            pid_t spawnPID = fork();
            switch (spawnPID) {
                case -1:
                    perror("fork()\n");
                    fflush(stdout);
                    break;
                case 0:
                    if (!curCmd->bg) {
                        sigaction(SIGINT, &fgsigint, NULL);
                    }
                    sigaction(SIGTSTP, &ignore_action, NULL);
                    fflush(stdout);
                    execvp(curCmd->argv[0],  curCmd->argv);
                    perror("execvp");
                    break;
                default:
                    if (curCmd->bg) {
                        activehead = addPID(activehead, spawnPID);
                        printf("background pid is %d\n", spawnPID);
                        fflush(stdout);
                        spawnPID = waitpid(spawnPID, &childStatus, WNOHANG);
                    } else {
                        spawnPID = waitpid(spawnPID, &childStatus, 0);
                    }
                    if (strlen(curCmd->infile) > 0) {
                        close(sourceFD);
                        int result = dup2(inFD, 0);
                        if (result == -1) {
                            perror("source dup2()");
                        }
                    }
                    if (strlen(curCmd->outfile) > 0) {
                        close(targetFD);
                        
                        int result = dup2(outFD, 1);
                        if (result == -1) {
                            perror("target dup2()");
                        }
                    }
                    break;
            }
        }

        for (int i = 0; i < curCmd->argc; i++) {
            free(curCmd->argv[i]);
        }    
        free(curCmd);
    }

    return 0;

}