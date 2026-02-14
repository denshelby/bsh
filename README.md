# bsh

`bsh` is a simple Linux shell program written in C. It provides basic shell functionality, including executing built-in commands like `cd`, `status`, and `exit`, executing external commands, handling input/output redirection, and managing background processes.

## Features

- **Built-in Command Execution**:
  - `cd`: Change the current working directory.
  - `status`: Display the exit status of the last foreground process.
  - `exit`: Exit the shell and terminate all background processes.
- **External Command Execution**: Supports running commands using the `execvp` system call.
- **Input/Output Redirection**: Allows redirection of input (`<`) and output (`>`).
- **Background Processes**: Supports running processes in the background using the `&` operator.
- **Signal Handling**:
  - Ignores `SIGINT` for the shell itself but allows foreground processes to handle it.
  - Toggles foreground-only mode using `SIGTSTP`.

## How It Works

1. **Input Parsing**: The shell prompts the user for input, parses the command, and identifies arguments, input/output redirection, and background execution.
2. **Built-in Commands**: If the command is a built-in (`cd`, `status`, `exit`), it is executed directly.
3. **External Commands**: For other commands, a child process is created using `fork()`, and the command is executed using `execvp`.
4. **Background Processes**: Background processes are managed using a linked list, and their statuses are periodically checked.
5. **Signal Handling**:
   - `SIGINT` (`Ctrl+C`) is ignored by the shell but passed to foreground processes.
   - `SIGTSTP` (`Ctrl+Z`) toggles between normal and foreground-only modes.

## File Structure

- **bsh.c**: Contains the main logic for the shell, including the command loop, process management, and signal handling.
- **bsh.h**: Header file defining the `command` and `activepid` structures, as well as function prototypes.
- **util.c**: Implements utility functions for parsing input, managing the process list, and handling signals.

## Compilation

To compile the program, use the make file:

```bash
make
```

## Usage

Once compiled, the executable can be found in the `bin/` directory. Run bsh from the bin director with:

```bash
./bsh
```

### Example Commands

- Run a foreground command:
  ```bash
  ls -l
  ```
- Run a background command:
  ```bash
  sleep 10 &
  ```
- Redirect input and output:
  ```bash
  sort < input.txt > output.txt
  ```
- Change directory:
  ```bash
  cd /path/to/directory
  ```
- Check the exit status of the last foreground process:
  ```bash
  status
  ```
- Exit the shell:
  ```bash
  exit
  ```

## License

GNU GPLv3