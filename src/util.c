#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "bsh.h"
#include "builtin.h"

bool bgtoggle = true;

/******************************************************************************
 * Function Name: getInput()
 * 
 * Description: Prompts user for a command to execute. Parses the input, creates
 * and returns a struct with the data.
 * 
 *****************************************************************************/
Command *getInput(CommandArray *cmds) {

    char input[MAXINPUT];
    input[0] = '\0';
    Command *curCmd = (Command *) calloc(1, sizeof(*curCmd));
    curCmd->argc = 0;

    // Input prompt
    printf("\033[1;36mbsh>\033[0m ");           
    fflush(stdout);
    fgets(input, sizeof(input), stdin);
    add_command(cmds, input);

    // Tokenize and parse input
    char *token = strtok(input, " \n");
    while (token) {
        if (!strcmp(token, "<")) {
            strcpy(curCmd->infile, strtok(NULL, " \n"));
        } else if (!strcmp(token, ">")) {
            strcpy(curCmd->outfile, strtok(NULL, " \n"));
        } else if (!strcmp(token, "&")) {
            if (bgtoggle) {
                curCmd->bg = true;
            }
        } else {
            curCmd->argv[curCmd->argc++] = token;
        }
        
        token = strtok(NULL, " \n");
    }

    return curCmd;
}

/******************************************************************************
 * Function Name: processCheck()
 * 
 * Description: Traverses the linked list of running processes and reports 
 * any that are finished.
 * 
 *****************************************************************************/
void processCheck(ActivePID *activehead) {
    ActivePID *activecur = activehead;
    pid_t curChild;
    int curStatus;

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
}

/******************************************************************************
 * Function Name: addPID()
 * 
 * Description: Allocates memory for a new pid and adds it to the linked list
 * of pids.
 * 
 *****************************************************************************/
ActivePID *addPID(ActivePID *head, pid_t pid) {
    ActivePID *newpid = (ActivePID *) calloc(1, sizeof(ActivePID));
    newpid->pid = pid;
    newpid->next = head;
    return newpid;
}

/******************************************************************************
 * Function Name: removePID()
 * 
 * Description: Removes a pid from the linked list of pids. Frees the memory
 * of the removed pid.
 * 
 *****************************************************************************/
void removePID(ActivePID *head, pid_t pid) {
    ActivePID *prevpid = head;
    ActivePID *curpid = head->next;

    if (head->pid == pid) {
        head = head->next;
        free(prevpid);
        return;
    }

    while (curpid != NULL) {
        if (curpid->pid == pid) {
            prevpid->next = curpid->next;
            free(curpid);
            return;
        }
        curpid = curpid->next;
    }

}

/******************************************************************************
 * Function Name: handle_fgsigint()
 * 
 * Description: SIGINT handler
 * 
 *****************************************************************************/
void handle_fgsigint(int signo) {
    exit(0);
}

/******************************************************************************
 * Function Name: handle_bgsigtstp()
 * 
 * Description: SIGTSTP handler
 * 
 *****************************************************************************/
void handle_bgsigtstp(int signo) {
    if (bgtoggle) {
        bgtoggle = false;
        char *message = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 50);
    } else {
        bgtoggle = true;
        char *message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 30);
    }
}