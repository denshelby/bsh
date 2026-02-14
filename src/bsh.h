#ifndef _BSH_
#define _BSH_

#define MAXINPUT 2048
#define MAXARG 512
#define MAXPATH 256
#define NUMCHILDREN 50


/******************************************************************************
 * Command - Struct to hold user input command and associated arguments
 *****************************************************************************/
typedef struct command {
    int argc;
    char *argv[MAXARG];
    char infile[MAXPATH];
    char outfile[MAXPATH];
    bool bg;
} Command;

/******************************************************************************
 * ActivePID - Struct to hold a pid and a pointer to the next pid in a linked
 * list.
 *****************************************************************************/
typedef struct activepid {
    pid_t pid;
    struct activepid *next;
} ActivePID;


Command *getInput();
// void printCmd(Command *);

ActivePID *addPID(ActivePID *, pid_t);
void removePID(ActivePID *, pid_t);

void handle_bgsigint(int);
void handle_bgsigtstp(int);

#endif