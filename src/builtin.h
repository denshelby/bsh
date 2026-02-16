#ifndef _BSH_BUILTIN_
#define _BSH_BUILTIN_

#include "bsh.h"

#define MAXCOMMANDS 10

// Forward declaration of Command
typedef struct command Command;

typedef struct command_array {
    char arr[MAXCOMMANDS][MAXINPUT];
    int start;
    int curr;
    int count;
} CommandArray;

void add_command(CommandArray *, char *);
void print_commands(CommandArray *);

void bsh_status(int *);
void bsh_cd(Command *);
void bsh_history(CommandArray *);

#endif      //_BSH_BUILTIN_