#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAXINPUT 2048
#define MAXARG 512
#define MAXPATH 256
#define NUMCHILDREN 50


/******************************************************************************
 * struct command
 * 
 *****************************************************************************/
struct command {
    int argc;
    char *argv[MAXARG];
    char infile[MAXPATH];
    char outfile[MAXPATH];
    bool bg;
};

struct command *getInput();
void printCmd(struct command *);

struct activepid {
    pid_t pid;
    struct activepid *next;
};

struct activepid *addPID(struct activepid *, pid_t);
void removePID(struct activepid *, pid_t);


void handle_bgsigint(int);
void handle_bgsigtstp(int);