#include "bsh.h"

/******************************************************************************
 * Program Name: bsh
 * 
 * Description: Linux shell.
 * 
 *
 *****************************************************************************/
int main() {
    int childStatus = 0;
    int inFD = dup(0);
    int outFD = dup(1);
    struct activepid *activehead = NULL;
    // activehead->next = NULL;
    struct activepid *activecur = activehead;
    int activecnt = 0;
    pid_t curChild;
    int curStatus;

    // Ignore SIGINT
    struct sigaction ignore_action = {0};
    ignore_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &ignore_action, NULL);

    // SIGINT kill bg processes
    struct sigaction bgsigint = {0};
    bgsigint.sa_handler = handle_bgsigint;
    bgsigint.sa_flags = 0;

    // SIGTSTP
    struct sigaction bgsigtstp = {0};
    bgsigtstp.sa_handler = handle_bgsigtstp;
    bgsigtstp.sa_flags = 0;
    sigaction(SIGTSTP, &bgsigtstp, NULL);


    while (true) {

        // Check running processes and report any that are finished
        activecur = activehead;
        while (activecur != NULL) {
            curChild = waitpid(activecur->pid, &curStatus, WNOHANG);
            if (curChild == activecur->pid) {
                if (WIFEXITED(curStatus)) {
                    printf("background pid %d is done: exit value %d\n", activecur->pid, WEXITSTATUS(curStatus));
                } else {
                    printf("background pid %d is done: terminated by signal %d\n", activecur->pid, WTERMSIG(curStatus));
                }
                fflush(stdout);
                removePID(activehead, activecur->pid);
            }
            activecur = activecur->next;
        }

        struct command *curCmd = getInput();

        // Ignore blank line or comment
        if (curCmd->argc == 0) continue;
        if (!strncmp(curCmd->argv[0], "#", 1)) continue;

        // Process built in commands
        if (!strcmp(curCmd->argv[0], "exit")) {
            free(curCmd);
            break;
        } else if (!strcmp(curCmd->argv[0], "cd")) {
            if (curCmd->argc > 1) {
                if (chdir(curCmd->argv[1]) == -1) {
                    printf("%s: No such file or directory\n", curCmd->argv[1]);
                    fflush(stdout);
                }
            } else {
                chdir(getenv("HOME"));
            }
        } else if (!strcmp(curCmd->argv[0], "status")) {
            printf("exit value %d\n", WEXITSTATUS(childStatus));
            fflush(stdout);
        
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
                    if (curCmd->bg) {
                        sigaction(SIGINT, &bgsigint, NULL);
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
        curCmd = NULL;
        free(curCmd);
    }

    return 0;

}