#ifndef _BSH_BUILTIN_
#define _BSH_BUILTIN_

#include "bsh.h"

#define MAXCOMMANDS 10

// Forward declaration of Command
typedef struct command Command;

void bsh_status(int *);
void bsh_cd(Command *);
void bsh_history();

#endif      //_BSH_BUILTIN_