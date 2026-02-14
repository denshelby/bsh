#include "bsh.h"

bool bgtoggle = true;

/******************************************************************************
 * Function Name: getInput()
 * 
 * Description: Prompts user for a command to execute. Parses the input, creates
 * and returns a struct with the data.
 * 
 *****************************************************************************/
struct command *getInput() {

    char input[MAXINPUT];
    input[0] = '\0';
    struct command *curCmd = (struct command *) calloc(1, sizeof(struct command));
    curCmd->argc = 0;

    // Input prompt
    printf(": ");           
    fflush(stdout);
    fgets(input, sizeof(input), stdin);

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


struct activepid *addPID(struct activepid *head, pid_t pid) {
    struct activepid *newpid = (struct activepid *) calloc(1, sizeof(struct activepid));
    newpid->pid = pid;
    newpid->next = head;
    return newpid;
}

void removePID(struct activepid *head, pid_t pid) {
    struct activepid *prevpid = head;
    struct activepid *curpid = head->next;

    if (head->pid == pid) {
        head = head->next;
        // free(prevpid);
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


void handle_bgsigint(int signo) {
    exit(0);
}


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